package jpsam3.dsa.huffman; /*
  *  FIT2004 Prac 5, Question 2
  *  Name:           Jonathan Poltak Samosir
  *  Email:          jpsam3@student.monash.edu
  *  Student ID:     2271 3603
  *  Date:           2012-10-15
  */

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.PriorityQueue;

public class HuffmanCodeCompressor
{
	private static final int ASCII_BIT_LEN = 8;
	private static final int ASCII_PRINTABLE_CHARS = 95;
	private static final int ASCII_DIFF = 32;

	private int totalChars;
	private int[] charFreqs;
	private String[] charCodes;

	public HuffmanCodeCompressor()
	{
		totalChars = 0;
		charFreqs = new int[ASCII_PRINTABLE_CHARS];
		charCodes = new String[ASCII_PRINTABLE_CHARS];
	}

	/*
	 * Reads the given file and sends each line to be handled by handleLine()
	 */
	public void readFile(String filePath)
	{
		String line;
		BufferedReader in;

		try {
			in = new BufferedReader(new FileReader(filePath));

			while ((line = in.readLine()) != null)			// read all lines in file
				handleLine(line);

			in.close();
		} catch (IOException e) {
			System.err.printf("Fatal IO error occured.\n");
		}
	}

	/*
	 * Handles the current line, incrementing each character's frequency if it's found in the line.
	 * Also adds the total number of characters in the line to the overall totalChars counter.
	 */
	private void handleLine(String strLine)
	{
		char[] line;

		line = strLine.toCharArray();
		totalChars += line.length;

		for (char c : line)			/* for each character in the line, if it's a printable ASCII char, increment frequency in freq_arr */
			if (c >= ASCII_DIFF && c <= 126)
				charFreqs[c-ASCII_DIFF]++;
	}

	/*
	 * Makes the array of characters into a priority queue.
	 */
	private PriorityQueue<Node> queueFromArray(int[] iArr)
	{
		PriorityQueue<Node> q;

		q = new PriorityQueue<Node>();
		for (int i = 0; i < iArr.length; i++)
			if (iArr[i] > 0)
				q.add(new Node(iArr[i], (char) (i+ASCII_DIFF)));

		return q;
	}

	/*
	 * Constructs binary tree from used characters by polling them from PriorityQueue.
	 * Places the root of the constructed tree in PriorityQueue q once finished.
	 * Will be a _complete_ binary tree.
	 */
	public void constructTree()
	{
		PriorityQueue<Node> q;

		q = queueFromArray(charFreqs);

		if (q.size() == 1)			// exception case for when only a single character is present in the file (can be multiple of the same character); makes it a child node of an empty parent (so it gets assigned a 1-bit code)
		{
			Node parent = new Node(0);

			Node solo = q.poll();
			parent.left = solo;
			q.add(parent);
		}

		while (q.size() != 1)		// builds the prefix tree
		{
			Node parent = new Node(0);

			Node leftChild = q.poll();
			parent.left = leftChild;

			Node rightChild = q.poll();
			parent.right = rightChild;

			parent.count = (leftChild.count + rightChild.count);		// give parent node.count the sum of children's counts

			q.add(parent);
		}

		traverseTree(q.poll(), "");
	}

	/*
	 * Traverses the tree that has been constructed recursively and assigns Huffman codes to each Node in the tree.
	 */
	private void traverseTree(Node node, String code)
	{
		if (node != null)			// node == null means we've reached the end of a path
		{
			charCodes[node.character-ASCII_DIFF] = code;		// add current code to this node
			traverseTree(node.left, code + "1");	// continue for left children
			traverseTree(node.right, code + "0");	// continue for right children
		}
	}

	/*
	 * Prints out the original size of the file and the new size of the file (after a Huffman code has been generated).
	 */
	public void printSizeOfFile()
	{
		float oldBytes, newBytes, savings;
		int i, subtotal, total;

		total = 0;
		oldBytes = (float) (((float) totalChars * (float) ASCII_BIT_LEN) / 8.0);
		System.out.printf("Original size of file: %.2f bytes\n", oldBytes);

		for (i = 0; i < ASCII_PRINTABLE_CHARS; i++)
			if (charFreqs[i] > 0)
			{
				subtotal = charCodes[i].length() * charFreqs[i];
				total += subtotal;
			}

		newBytes = (float) ((float) total / 8.0);
		System.out.printf("Compressed size of file: %.2f bytes\n", newBytes);

		savings = (1-(newBytes/ oldBytes)) * 100;
		System.out.printf("Savings of %.2f%%!\n", savings);
	}

	/*
	 * Prints out the characters used along with their new Huffman codes.
	 */
	public void printCodes()
	{
		int i;

		for (i = 0; i < ASCII_PRINTABLE_CHARS; i++)
			if (charFreqs[i] > 0)
				System.out.printf("%c : %s\n", (char) i+ASCII_DIFF, charCodes[i]);
	}

	public static void main(String[] args)
	{
		HuffmanCodeCompressor test = new HuffmanCodeCompressor();

		test.readFile("jpsam3/dsa/huffman/huffman.txt");
		test.constructTree();
		test.printSizeOfFile();
		System.out.printf("\n");
		test.printCodes();
	}

	public class Node implements Comparable<Node>
	{
		public int count;
		public char character;
		public Node left;
		public Node right;

		public Node(int initCount, char initChar, Node initLeft, Node initRight)
		{
			count = initCount;
			character = initChar;
			left = initLeft;
			right = initRight;
		}

		public Node(int initCount, char initChar)
		{
			this(initCount, initChar, null, null);
		}


		public Node(int initCount)
		{
			this(initCount, ' ', null, null);
		}

		@Override
		public int compareTo(Node n)
		{
			int compValue;

			compValue = this.count - n.count;
			return compValue;
		}
	}
}
