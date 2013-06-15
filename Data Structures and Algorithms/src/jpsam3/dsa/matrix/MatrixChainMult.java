package jpsam3.dsa.matrix; /*
  *  FIT2004 Prac 5, Question 2
  *  Name:           Jonathan Poltak Samosir
  *  Email:          jpsam3@student.monash.edu
  *  Student ID:     2271 3603
  *  Date:           2012-10-15
  */

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class MatrixChainMult
{
	private int[] matrixDimensions;

	/*
	 * Reads the specified file and hands each line over to handleLine().
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
			System.exit(-1);
		}
	}

	/*
	 * Converts the line to a number of integers that will be filled into matrixDimensions[].
	 */
	private void handleLine(String line)
	{
		String[] dimensions;
		int i;

		i = 0;
		dimensions = line.split(" ");

		matrixDimensions = new int[dimensions.length];

		for (String s : dimensions)
			matrixDimensions[i++] = Integer.parseInt(s);
	}

	/*
	 * Brain overheated trying to understand this; consult in-line comments for easier understanding.
	 *
	 * Should be O(n^3) time algorithm, which may seem to take more time than the non-dynamic programming version but given the constants involved
	 * it's probably faster to do the calculations to find out the optimal multiplication ordering than performing the actual matrix multiplications.
	 */
	public void getOptimalMults()
	{
		int inputLen, chainLen, noOfMults;
		int i;
		int left, right;
		int[][] multTable;

		inputLen = matrixDimensions.length -1;						// no. of ints in the input file

		multTable = new int[inputLen][inputLen];					// create a table to store multiplication products of specified matrices, i.e., multTable[i][n] = # of multiplications in Ai*Ai+1*...*An

		for (i = 0; i < inputLen; i++)								// initialise all in multTable to 0
			multTable[i][i] = 0;


		for (chainLen = 1; chainLen <= inputLen; chainLen++)		// for each matrix chain variation
		{
			for (left = 0; left < (inputLen-chainLen); left++)		// for each position that is relevant to current variation of matrix chain (there will be inputLen-chainLen indices to work with in this subsection)
			{
				right = left + chainLen;							// to ensure right index actually starts from the right most value of left
				multTable[left][right] = Integer.MAX_VALUE;			// set the value of the current index to the max value as we can then update it to the min value we get after calculating

				for (i = left; i < right; i++)						// for each position between left and right
				{
					noOfMults = multTable[left][i] + multTable[i+1][right] + matrixDimensions[left] * matrixDimensions[i+1] * matrixDimensions[right+1];		// our actual calculation to work out the given no. of mutliplications for the given matrices

					if (noOfMults < multTable[left][right])			// only add this calculated value to the table if it is more optimal than the previously stored value
						multTable[left][right] = noOfMults;
				}
			}
		}
		printMostOptimal(multTable);
	}

	/*
	 * Prints out most optimal amount of multiplications using the table of multiplications.
	 */
	private void printMostOptimal(int[][] table)
	{
		int firstRow = 0;
		int lastCol = table.length -1;
		System.out.printf("Minimum amount of multiplications: %d\n", table[firstRow][lastCol]);		// print out value in first row, last column (due to the way the table is computed, this will be the min amount of multiplications)
	}

	public static void main(String[] args)
	{
		MatrixChainMult test = new MatrixChainMult();

		test.readFile("matrix.txt");
		test.getOptimalMults();
	}
}
