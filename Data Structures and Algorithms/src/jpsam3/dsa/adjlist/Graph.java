package jpsam3.dsa.adjlist;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.TreeMap;
import java.util.TreeSet;

public class Graph
{
	private TreeMap<String,TreeSet<String>> adjList;

	public Graph()
	{
		adjList = new TreeMap<String, TreeSet<String>>();
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
			System.err.printf("Fatal IO error occured.");
		}
	}

	/*
	 * Handles readLine by inserting node and specified edge into adjList if not already there.
	 * ***ASSUMES proper formatting (as in prac specifications PDF)***
	 */
	public void handleLine(String readLine)
	{
		String[] names;
		TreeSet<String> friendSet;

		names = readLine.split(" : ");

		for (String name : names)				// for each name, it adds it as a vertex in the jpsam3.dsa.adjlist.Graph (if not already existing)
			if (!adjList.containsKey(name))
				addNewVertex(name);

		friendSet = adjList.get(names[0]);
		if (!friendSet.contains(names[1]))		// adds new friend (edge) to given name's friend list if not already there
			addNewFriend(friendSet, names[1]);

		friendSet = adjList.get(names[1]);
		if (!friendSet.contains(names[0]))		// same as above, but for second name in line
			addNewFriend(friendSet, names[0]);
	}

	/*
	 * Simple method to add a new vertex to our adjacency list.
	 * Uses TreeMap class' put() method as no real need to reimplement the op.
	 */
	public void addNewVertex(String key)
	{
		TreeSet<String> friendSet;

		friendSet = new TreeSet<String>();
		adjList.put(key, friendSet);
	}

	/*
	 * Simple method to add new friend (or in theory an edge between vertices).
	 * Uses TreeSet class' add() method as no real need to reimplement the op.
	 */
	public void addNewFriend(TreeSet<String> friendSet, String key)
	{
		friendSet.add(key);
	}

	/*
	 * Prints out the information contained in the jpsam3.dsa.adjlist.Graph in a human-readable format.
	 * For each vertex in the adjacency list, prints out the vertex followed by its adjacent vertices.
	 ***Assuming that something is put into the TreeMap first.***
	 ***Haven't put error checking in as it distracts from the actual implementation and use of the data structures.***
	 */
	public void printGraph()
	{
		String currentVertex, currentFriend;
		TreeSet<String> currentFriendSet;

		currentVertex = adjList.firstKey();				// start at first vertex in jpsam3.dsa.adjlist.Graph

		do
		{
			System.out.printf("%s -->", currentVertex);
			currentFriendSet = adjList.get(currentVertex);
			currentFriend = currentFriendSet.first();	// start at first adjacent vertex/friend

			do
				System.out.printf("   %s", currentFriend);
			while ((currentFriend = currentFriendSet.higher(currentFriend)) != null);		// updates the currentFriend each iteration to the next adjacent vertex to currentVertex

			System.out.printf("\n");
		}
		while ((currentVertex = adjList.higherKey(currentVertex)) != null);				// updates the currentVertex each iteration to the next vertex in the jpsam3.dsa.adjlist.Graph (until none left)
	}

	public static void main(String[] args)
	{
		Graph testG;

		testG = new Graph();
		testG.readFile("names.txt");
		testG.printGraph();
	}

}