package jpsam3.dsa.bipartitegraph;
/*
 *  Name:           Jonathan Poltak Samosir
 *  Email:          jpsam3@student.monash.edu
 *  Date:           2012-10-15
 */

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.TreeMap;
import java.util.TreeSet;

public class BipartiteGraphChecker
{
	private TreeMap<String,Vertex> adjList;

	public BipartiteGraphChecker()
	{
		adjList = new TreeMap<String,Vertex>();
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
	 * Handles readLine by inserting Vertex and specified friends into adjList if not already there.
	 * ***ASSUMES proper formatting (as in prac specifications PDF)***
	 */
	private void handleLine(String readLine)
	{
		String[] vertices;
		Vertex vertexA, vertexB;

		vertices = readLine.split(" ");

		for (String vertex : vertices)					// for each vertex, it adds it as a new vertex in the Graph (if not already existing)
			if (!adjList.containsKey(vertex))
				addNewVertex(vertex);

		vertexA = adjList.get(vertices[0]);
		vertexB = adjList.get(vertices[1]);

		if (!vertexA.alreadyNeighbours(vertexB))		// adds new edge to given name's friend list if not already there
			vertexA.addNewNeighbour(vertexB);

		if (!vertexB.alreadyNeighbours(vertexA))		// same as above, but for second name in line
			vertexB.addNewNeighbour(vertexA);
	}

	/*
	 * Simple method to add a new vertex to our adjacency list.
	 */
	private void addNewVertex(String key)
	{
		Vertex toAdd;

		toAdd = new Vertex(key);
		adjList.put(key, toAdd);
	}

	/*
	 * Prints out the information contained in the Graph in a human-readable format.
	 * For each vertex in the adjacency list, prints out the vertex followed by its adjacent vertices.
	 ***Assuming that something is put into the TreeMap first.***
	 ***Haven't put error checking in as it distracts from the actual implementation and use of the data structures.***
	 */
	public void printGraph()
	{
		Vertex currentVertex;
		TreeSet<Vertex> currentFriendSet;
		String currentString;

		System.out.printf("Adjacency list representation:\n");
		currentString = adjList.firstKey();
		do
		{
			currentVertex = adjList.get(currentString);
			System.out.printf("%s -->", currentVertex.name);

			currentFriendSet = currentVertex.neighbours;
			for (Vertex friend : currentFriendSet)
				System.out.printf("   %s", friend.name);
			System.out.printf("\n");
		}
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	/*
	 * Returns the next Vertex in adjList with visited = false. If none, returns null.
	 */
	private Vertex nextNotVisited()
	{
		Vertex currentVertex;
		String currentString;

		currentString = adjList.firstKey();
		do
		{
			currentVertex = adjList.get(currentString);

			if (!currentVertex.visited)
				return currentVertex;
		}
		while ((currentString = adjList.higherKey(currentString)) != null);

		return null;			// null if there is none
	}

	/*
	 * all Vertices in adjList TreeMap are visited ? true : false
	 * Uses nextNotVisited() to determine if any Vertex is not visited as it returns null if all visited.
	 */
	private boolean allVisited()
	{
		if (nextNotVisited() == null)
			return true;
		return false;
	}

	/*
	 * Resets all Vertices in adjList to default state of visited = false.
	 */
	private void resetVisited()
	{
		Vertex currentVertex;
		String currentString;

		currentString = adjList.firstKey();
		do
		{
			currentVertex = adjList.get(currentString);
			currentVertex.visited = false;
		}
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	/*
	 * Detects whether or not graph is bipartite. Graph does not have to be all connected.
	 */
	public void bipartiteDetect()
	{
		Vertex v;
		boolean isBipartite;

		isBipartite = true;
		resetVisited();

		while (!allVisited())
		{
			v = nextNotVisited();
			v.colour = Colour.BLUE;

			if (!DFS(v))
			{
				isBipartite = false;
				break;
			}
		}

		if (isBipartite)
			System.out.printf("Graph is bipartite.\n");
		else
			System.out.printf("Graph is _not_ bipartite.\n");
	}

	/*
	 * Just a slightly modified DFS on the graph to making it two coloured and at the same time checking colours of neighbour Vertices.
	 * Returns false whenever any violation of bipartite rules are broken.
	 * Returns true if no bipartite rules get broken.
	 */
	private boolean DFS(Vertex v)
	{
		TreeSet<Vertex> neighbourSet;

		v.visited = true;
		neighbourSet = v.neighbours;

		for (Vertex neighbour : neighbourSet)
		{
			if (!neighbour.visited)
			{
				neighbour.colour = v.nextColour();	/* colour current neighbour with other colour to v */
				if (DFS(neighbour))
					continue; 						/* continue to next Vertex in neighbourSet if everything is good for current */
				else
					return false;
			} else if (neighbour.colour.equals(v.colour))		/* colour is the same to adjacent Vertex */
				return false;
		}
		return true;			/* only reaches here if for each neighbour of v, they don't violate bipartite rules */
	}



	public static void main(String[] args)
	{
		BipartiteGraphChecker testG;

		testG = new BipartiteGraphChecker();
		testG.readFile("edges.txt");
		testG.printGraph();
		System.out.printf("\n");
		testG.bipartiteDetect();
	}

	public enum Colour {RED, BLUE};

	/*
	 * Used for creating Vertex objects. They each have a name and visited attributes.
	 * They also contain a TreeSet of Vertices called neighbours which essentially provides an adjacency list implementation for Graphs
	 * as the Graph object will be made as a TreeMap which maps Strings to Vertices (each Vertex containing a TreeSet of friend Vertices).
	 */
	public class Vertex implements Comparable<Vertex>
	{
		public TreeSet<Vertex> neighbours;			// adjacency list for friends
		public String name;
		public boolean visited;
		public Colour colour;

		public Vertex(String initName)
		{
			name = initName;
			visited = false;
			neighbours = new TreeSet<Vertex>();
		}

		/*
		 * Checks if this Vertex is already friends with friend Vertex.
		 */
		public boolean alreadyNeighbours(Vertex neighbour)
		{
			if (neighbours.contains(neighbour))
				return true;
			return false;
		}

		/*
		 * Really simple method. Used for making the handleLine() code clearer.
		 */
		public void addNewNeighbour(Vertex newNeighbour)
		{
			neighbours.add(newNeighbour);
		}

		/*
		 * Returns the next alternating Colour.
		 */
		public Colour nextColour()
		{
			if (this.colour == Colour.RED)
				return Colour.BLUE;
			return Colour.RED;
		}

		/*
		 * Allows Vertex objects to be compared by their name (mainly used for ordering in TreeMap/TreeSets).
		 */
		@Override
		public int compareTo(Vertex o)
		{
			int nameCompVal;

			nameCompVal = this.name.compareTo(o.name);
			return nameCompVal;
		}

	}
}