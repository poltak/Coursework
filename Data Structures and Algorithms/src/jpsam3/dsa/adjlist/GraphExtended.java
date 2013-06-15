package jpsam3.dsa.adjlist;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class GraphExtended
{
	private static final int MAX_GROUPS = 10;
	private static final int INF = -1;

	private TreeMap<String,Vertex> adjList;

	public GraphExtended()
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
	public void handleLine(String readLine)
	{
		String[] names;
		Vertex friendA, friendB;

		names = readLine.split(" : ");

		for (String name : names)					// for each name, it adds it as a vertex in the jpsam3.dsa.adjlist.Graph (if not already existing)
			if (!adjList.containsKey(name))
				addNewVertex(name);

		friendA = adjList.get(names[0]);
		friendB = adjList.get(names[1]);

		if (!friendA.alreadyFriends(friendB))		// adds new friend (edge) to given name's friend list if not already there
			friendA.addNewFriend(friendB);

		if (!friendB.alreadyFriends(friendA))		// same as above, but for second name in line
			friendB.addNewFriend(friendA);
	}

	/*
	 * Simple method to add a new vertex to our adjacency list.
	 */
	public void addNewVertex(String key)
	{
		Vertex toAdd;

		toAdd = new Vertex(key);
		adjList.put(key, toAdd);
	}

	/*
	 * Prints out the information contained in the jpsam3.dsa.adjlist.Graph in a human-readable format.
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

			currentFriendSet = currentVertex.friends;
			for (Vertex friend : currentFriendSet)
				System.out.printf("   %s", friend.name);
			System.out.printf("\n");
		}
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	/*
	 * Returns the next Vertex in adjList with visited = false. If none, returns null.
	 */
	public Vertex nextNotVisited()
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
	public boolean allVisited()
	{
		if (nextNotVisited() == null)
			return true;
		return false;
	}

	/*
	 * Resets all Vertices in adjList to default state of visited = false.
	 */
	public void resetVisited()
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
	 * Handler method for the finding of groups. Calls BFS a number of times to find each group before calling printing method.
	 * Also handles BFS's reseting of all Vertices' visit attribute (as we only want to call it once, not for each time BFS is called).
	 * 		O(|V|) * O(BFS)
	 * 	  =	O(|V| (|V| + |E|))
	 */
	public void findGroups()
	{
		ArrayList<TreeSet<Vertex>> setOfGroups;
		Vertex currentVertex;

		setOfGroups = new ArrayList<TreeSet<Vertex>>(MAX_GROUPS);	// makes ArrayList of TreeSets to contain friend groups with default size
		resetVisited();												// resets visited Vertices from previous operations

		currentVertex = adjList.get(adjList.firstKey());			// first vertex to operate on
		do
		{
			setOfGroups.add(DFS(currentVertex));					// do BFSs to find groups until all found
			currentVertex = nextNotVisited();
		} while (!allVisited());

		printGroups(setOfGroups);									// prints outcome
	}

	/*
	 * Implements a customised breadth-first search; instead of messing with distances, as they're not much use for us, it uses boolean values
	 * for visited (whether the Vertex has been visited or not). Uses a LinkedList for FIFO Queue operations.
	 *
	 * Returns TreeSet of Vertices of which all have been visited during this BFS, which will be a friendship group.
	 *
	 * O(|V| + |E|)
	 */
	@SuppressWarnings("unused")
	private TreeSet<Vertex> BFS(Vertex v)
	{
		Queue<Vertex> vQueue;
		TreeSet<Vertex> friendSet, visitedVertices;				// visitedVertices stores a set of all Vertices we have visited in this BFS
		Vertex currentVertex;

		visitedVertices = new TreeSet<Vertex>();
		vQueue = new LinkedList<Vertex>();

		v.visited = true;			// makes sure to mark the first Vertex as visited
		visitedVertices.add(v);		// makes sure to include the first Vertex into found group
		vQueue.add(v);

		while (!vQueue.isEmpty())
		{
			currentVertex = vQueue.remove();

			friendSet = currentVertex.friends;
			for (Vertex friend : friendSet)
			{
				if (!friend.visited)
				{
					friend.visited = true;
					visitedVertices.add(friend);
					vQueue.add(friend);											// and add to queue to be processed later in loop
				}
			}
		}

		return visitedVertices;
	}

	public TreeSet<Vertex> DFS(Vertex v)
	{
		TreeSet<Vertex> visitedVertices;

		visitedVertices = new TreeSet<Vertex>();
		return DFS(v, visitedVertices);
	}

	private TreeSet<Vertex> DFS(Vertex v, TreeSet<Vertex> visitedVertices)
	{
		TreeSet<Vertex> friendSet;

		v.visited = true;
		friendSet = v.friends;

		for (Vertex friend : friendSet)
		{
			if (!friend.visited)
			{
				visitedVertices.add(friend);
				return DFS(friend, visitedVertices);
			}
		}
		return visitedVertices;
	}

	/*
	 * Specialised print method for printing out friendship groups.
	 * Iterates through the List of groups and prints out names for each Vertex in a group (separating groups by line break).
	 */
	private void printGroups(ArrayList<TreeSet<Vertex>> groups)
	{
		int groupNo = 1;

		System.out.printf("Friendship groups:\n");
		for (TreeSet<Vertex> currentGroup : groups)
		{
			if (!currentGroup.isEmpty())
			{
				System.out.printf("Group %d:  ", groupNo++);

				for (Vertex currentVertex : currentGroup)
					System.out.printf("%s  ", currentVertex.name);
				System.out.printf("\n");
			}
		}
	}

	/*
	 * Implements unweighted shortest path algorithm to set the distances of each Vertex in given graph in relation to Vertex v.
	 * This is essentially a modified __BFS__ which updates distances relative to the starting Vertex each iteration.
	 *
	 * O(|V| + |E|)
	 */
	public void unweightedDistances(Vertex v)
	{
		Queue<Vertex> vQueue;
		Vertex currentVertex;
		TreeSet<Vertex> friendSet;

		vQueue = new LinkedList<Vertex>();

		resetDistances();			// O(|V|) time
		v.distance = 0;
		vQueue.add(v);

		while (!vQueue.isEmpty())			// O(|V|) time
		{
			currentVertex = vQueue.remove();
			friendSet = currentVertex.friends;

			for (Vertex friend : friendSet)						// for each friend of currentVertex--  O(|E|) time
			{
				if (friend.distance == INF)
				{
					friend.distance = currentVertex.distance +1;
					vQueue.add(friend);
				}
			}
		}

		printRelativeDistances(v);			// not related to actual distances algorithm
	}

	/*
	 * Printing method specific to printing out Vertex distances relative to Vertex v.
	 */
	private void printRelativeDistances(Vertex v)
	{
		Vertex currentVertex;
		String currentString;
		int dist;

		System.out.printf("Relative distances:\n");
		currentString = adjList.firstKey();
		do									// for each Vertex in adjList, print distances
		{
			currentVertex = adjList.get(currentString);
			System.out.printf("%s's distance from %s: ", v.name, currentVertex.name);
			if ((dist = currentVertex.distance) == INF)
				System.out.printf("INFINITE\n");
			else
				System.out.printf("%d\n", dist);
		}
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	/*
	 * For each Vertex in adjList, resets the distance attribute to default value of INF.
	 */
	private void resetDistances()
	{
		Vertex currentVertex;
		String currentString;

		currentString = adjList.firstKey();
		do
		{
			currentVertex = adjList.get(currentString);
			currentVertex.distance = INF;
		}
		while ((currentString = adjList.higherKey(currentString)) != null);
	}

	public static void main(String[] args)
	{
		GraphExtended testG;

		testG = new GraphExtended();
		testG.readFile("names.txt");
		testG.printGraph();
		System.out.printf("\n\n");
		testG.findGroups();
		System.out.printf("\n\n");
		testG.unweightedDistances(testG.adjList.get("Bernd"));
	}

	/*
	 * Used for creating Vertex objects. They each have a name, distance and visited attributes.
	 * They also contain a TreeSet of Vertices called friends which essentially provides an adjacency list implementation for Graphs
	 * as the jpsam3.dsa.adjlist.Graph object will be made as a TreeMap which maps Strings to Vertices (each Vertex containing a TreeSet of friend Vertices).
	 */
	public class Vertex implements Comparable<Vertex>
	{
		public TreeSet<Vertex> friends;			// adjacency list for friends
		public String name;
		public int distance;
		public boolean visited;

		public Vertex(String initName)
		{
			name = initName;
			visited = false;
			distance = INF;
			friends = new TreeSet<Vertex>();
		}

		/*
		 * Checks if this Vertex is already friends with friend Vertex.
		 */
		public boolean alreadyFriends(Vertex friend)
		{
			if (friends.contains(friend))
				return true;
			return false;
		}

		/*
		 * Really simple method. Used for making the handleLine() code clearer.
		 */
		public void addNewFriend(Vertex newFriend)
		{
			friends.add(newFriend);
		}

		/*
		 * Allows Vertex objects to be compared by their name (mainly used for ordering in TreeMap/TreeSets).
		 */
		@Override
		public int compareTo(Vertex o)
		{
			int nameCompVal;

			nameCompVal = name.compareTo(o.name);
			return nameCompVal;
		}
	}
}