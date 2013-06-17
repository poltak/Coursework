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
    private enum Colour {RED, BLUE}

	private final TreeMap<String,Vertex> adjList;

    public BipartiteGraphChecker() { adjList = new TreeMap<>(); }

    /**
     * Reads the given file and sends each line to be handled by handleLine()
     * @param filePath Path to file containing data.
     */
	public void handleFile(String filePath) throws RuntimeException
    {
		try {
			BufferedReader in = new BufferedReader(new FileReader(filePath));

            String line;
			while ((line = in.readLine()) != null)	// Extract data from each line.
				handleLine(line);

			in.close();
		} catch (IOException e) {
			System.err.printf("Fatal IO error occurred.");
            e.printStackTrace();
            throw new RuntimeException();
		}
	}

    /**
     * Handles readLine by inserting Vertex and specified friends into adjList if not already there.
     * @param readLine Line to be handled as String.
     */
	private void handleLine(String readLine)
	{
		String[] vertices = readLine.split(" ");

		for (String vertex : vertices)				// For each Vertex, add to adjList if not already there.
			if (!adjList.containsKey(vertex))
				addNewVertex(vertex);

		Vertex vertexA = adjList.get(vertices[0]);
		Vertex vertexB = adjList.get(vertices[1]);

		if (!vertexA.alreadyNeighbours(vertexB))	// Adds new edge to given name's friend list if not already there.
			vertexA.addNewNeighbour(vertexB);

		if (!vertexB.alreadyNeighbours(vertexA))	// Same as above, but for second name in line.
			vertexB.addNewNeighbour(vertexA);
	}

    /**
     * Simple method to add a new vertex to our adjacency list.
     * @param key Key found in data file.
     */
	private void addNewVertex(String key)
	{
		Vertex toAdd = new Vertex(key);
		adjList.put(key, toAdd);
	}

	/**
	 * Prints out the information contained in the Graph in a human-readable format.
	 * For each vertex in the adjacency list, prints out the vertex followed by its adjacent vertices.
	 ***Assuming that something is put into the TreeMap first.***
	 ***Haven't put error checking in as it distracts from the actual implementation and use of the data structures.***
	 */
	public void printGraph()
	{
		System.out.printf("Adjacency list representation:%n");
		String currentString = adjList.firstKey();
		do
		{
			Vertex currentVertex = adjList.get(currentString);
			System.out.printf("%s -->", currentVertex.name);

			TreeSet<Vertex> currentFriendSet = currentVertex.neighbours;
			for (Vertex friend : currentFriendSet)
            	System.out.printf("   %s", friend.name);

			System.out.printf("%n");
		} while ((currentString = adjList.higherKey(currentString)) != null);
	}

    /**
     * Finds and returns the next Vertex in adjList with visited == false. If none, returns null.
     * @return Next unvisited Vertex object or null if all visited.
     */
	private Vertex nextNotVisited()
	{
		String currentString = adjList.firstKey();
		do
		{
			Vertex currentVertex = adjList.get(currentString);
			if (!currentVertex.visited)
                return currentVertex;
		} while ((currentString = adjList.higherKey(currentString)) != null);

		return null;			// Return null if all visited.
	}

    /**
     * Uses nextNotVisited() to determine if any Vertex is not visited as it returns null if all visited.
     * @return Boolean value denoting whether or not all Vertices in graph have been visited.
     */
	private boolean allVisited() { return nextNotVisited() == null; }

	/**
	 * Resets all Vertices in adjList to default state of visited = false.
	 */
	private void resetVisited()
	{
		String currentString = adjList.firstKey();
		do
            adjList.get(currentString).visited = false;
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	/**
	 * Detects whether or not graph is bipartite. Graph does not have to be all connected.
	 */
	public void bipartiteDetect()
	{
		boolean isBipartite = true;
		resetVisited();

		while (allVisited())
		{
			Vertex v = nextNotVisited();
			v.colour = Colour.BLUE;

			if (!DFS(v))
			{
				isBipartite = false;
				break;
			}
		}

		if (isBipartite)	System.out.printf("Graph is bipartite.%n");
		else                System.out.printf("Graph is _not_ bipartite.%n");
	}

    /**
     * A slightly modified DFS algorithm on the graph to making it two-coloured and at the same time checking colours of
     * neighbouring Vertices.
     * @param v Vertex to perform DFS on.
     * @return Boolean value denoting whether or not the bipartite graph rules have been violated.
     */
	private boolean DFS(Vertex v)
	{
		TreeSet<Vertex> neighbourSet = v.neighbours;
        v.visited = true;

		for (Vertex neighbour : neighbourSet)
		{
			if (!neighbour.visited)
			{
				neighbour.colour = v.nextColour();	        // Colour current neighbour with different colour to v.
				if (!DFS(neighbour))    return false;
			} else if (neighbour.colour.equals(v.colour))	// If colour is the same as adjacent Vertex.
            {
				return false;
            }
		}
		return true;	// Only reaches here if for each neighbour of v, they don't violate bipartite rules.
	}



    /* Some tests */
	public static void main(String[] args)
	{
		BipartiteGraphChecker testG = new BipartiteGraphChecker();
		testG.handleFile("src/jpsam3/dsa/bipartitegraph/edges.txt");
		testG.printGraph();
		System.out.printf("%n");
		testG.bipartiteDetect();
	}

	/**
	 * Used for creating Vertex objects. They each have a name and 'visited' attributes.
	 * They also contain a TreeSet of Vertices called neighbours which essentially provides an adjacency list
     * implementation for Graphs as the Graph object will be made as a TreeMap which maps Strings to Vertices
     * (each Vertex containing a TreeSet of friend Vertices).
	 */
	private static class Vertex implements Comparable<Vertex>
	{
		private TreeSet<Vertex> neighbours;		// Adjacency list for friends.
		private final String    name;
        private Colour          colour;
		private boolean         visited;

		private Vertex(String initName)
		{
            neighbours = new TreeSet<>();
			name       = initName;
			visited    = false;
		}

        /**
         * Checks if this Vertex is already friends with neighbour Vertex.
         * @param neighbour Neighbouring Vertex to check friendship status against.
         * @return Boolean value denoting whether or not this Vertex is friends with neighbour Vertex.
         */
		private boolean alreadyNeighbours(Vertex neighbour) { return neighbours.contains(neighbour); }

        /**
         * Really simple method used for making the handleLine() code clearer and more reader-friendly.
         * @param newNeighbour Vertex to add to this.neighbours.
         */
		private void addNewNeighbour(Vertex newNeighbour) { neighbours.add(newNeighbour); }

        /**
         * @return Next colour from this.colour.
         */
		private Colour nextColour()
		{
			if (this.colour == Colour.RED)
				return Colour.BLUE;
			return Colour.RED;
		}

        /**
         * Allows Vertex objects to be compared by their name (mainly used for ordering in TreeMap/TreeSets).
         * @param o Vertex to compare this to.
         * @return Integer value following the standard Java compareTo() return values.
         */
        @Override
		public int compareTo(Vertex o) { return this.name.compareTo(o.name); }
	}
}