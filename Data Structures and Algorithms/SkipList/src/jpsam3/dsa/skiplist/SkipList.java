package jpsam3.dsa.skiplist;

import jpsam3.dsa.skiplist.Node.Level;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/*
 * TODO fully implement and test search and insert functions
 */
public class SkipList
{
	private int     numNodes;
    private Node    head;
    private Node[]  nodes;

	public SkipList(int[] keys)
	{
        numNodes = keys.length;
        nodes    = new Node[numNodes];

        // Initialise nodes in SkipList.
        for (int i = 0; i < numNodes; i++)	nodes[i] = new Node(keys[i]);
        setNodeLinks();
        for (int i = 0; i < numNodes; i++)	nodes[i].fillNode();

        head = nodes[0];
	}

	private void setNodeLinks()
	{
        Node next, prev;
	    for (int i = 0; i < numNodes; i++)
        {
            if (i-1 < 0)            prev = null;    // If first Node, don't point to other Node.
            else                    prev = nodes[i-1];

            if (i+1 >= numNodes)    next = null;    // If last Node, don't point to other Node.
            else                    next = nodes[i+1];

            nodes[i].setNodePointers(prev, next);   // Set pointers to appropriate Nodes.
        }
	}

	public boolean search(int searchKey)
	{
        return !(searchKey < 0 || searchKey > 999 || search(searchKey, head).getKey() != searchKey);
    }

    /**
     *
     * @param searchKey
     * @param currentNode
     * @return if searchKey is found, returns the Node with that key, else returns null
     */
    private Node search(int searchKey, Node currentNode)
    {
        Level[] levelArr = currentNode.getLevels();
        int levelCounter = currentNode.getHeight() -1;

        if (currentNode.getKey() <= searchKey)		// Don't print path of keys larger than searchKey.
        	System.out.println(currentNode.getKey());

        if (currentNode.getKey() == searchKey)    return currentNode;
        else
        {
            while (levelArr[levelCounter].next == null || levelArr[levelCounter].next.getKey() > searchKey)
            {
                if (levelCounter != 0)	levelCounter--;
                else                    return currentNode;
            }
            return search(searchKey, levelArr[levelCounter].next);
        }
    }

    /**
     * Allows deleting a Node by key.
     * @param deleteKey Specified key of Node to delete.
     * @return Boolean value denoting whether or not the deletion was successful.
     */
    public boolean delete(int deleteKey)
    {
    	Node deleteNode = search(deleteKey, head);

    	if (deleteNode.getKey() != deleteKey)	return false;
    	delete(deleteNode);
        return true;
    }

    /**
     * Deletes specified Node.
     * @param deleteNode Node to delete.
     */
    private void delete(Node deleteNode)
    {
        Node currentLeft  = deleteNode.getPredecessor();
        Node currentRight = deleteNode.getSuccessor();

        // Sort out left links to point to next Node.
        if (currentLeft != null)
        {
            currentLeft.setNodePointers(currentLeft.getPredecessor(), deleteNode.getSuccessor());
            currentLeft.fillNode();
        } else // If we're deleting left-most Node.
        {
        	currentRight.setNodePointers(null, currentRight.getSuccessor());
        	currentRight.fillNode();
        	head = head.getSuccessor();
        }
        // Sort out right links to point to prev Node.
        if (currentRight != null)
        {
            currentRight.setNodePointers(deleteNode.getPredecessor(), currentRight.getSuccessor());
            currentRight.fillNode();
        } else // If we're deleting right-most Node.
        {
        	currentLeft.setNodePointers(currentLeft.getPredecessor(), null);
        	currentLeft.fillNode();
        }

        printSkipList();
    }

    /**
     * Allows inserting a Node by key.
     * @param insertKey Specified key of Node to insert.
     * @return Boolean value denoting whether or not the insertion was successful.
     */
    public boolean insert(int insertKey)
    {
    	Node searchResult = search(insertKey, head);

    	if (searchResult.getKey() == insertKey)     return false;   // If Node already exists.
    	insert(insertKey, searchResult.getPredecessor());
    	return true;
    }

    /**
     * TODO lots of implemntation
     * Inserts
     * @param insertKey
     * @param prevNode
     */
	private void insert(int insertKey, Node prevNode)
    {
    	int i = getNextEmptyIndex();
    	Node nextNode = prevNode.getSuccessor();
    	Node predecessor;

    	nodes[i] = new Node(insertKey);
    	if (prevNode != null) {
    		prevNode.setNodePointers(prevNode.getPredecessor(), nodes[i]);
    		predecessor = prevNode;
    	} else {
    		predecessor = null;
    	}

    	if (nextNode != null) {
    		nextNode.setNodePointers(nodes[i], nextNode.getSuccessor());

    	} else {

    	}

    }

    /**
     * Finds and returns the index of the first found null Node.
     * @return Int value specifying Node index.
     */
    public int getNextEmptyIndex()
    {
        for (int i = 0; i < nodes.length; i++)
        	if (nodes[i] == null)   return i;
        return -1;
    }

    /**
     * Simply prints out SkipList Nodes as an ASCII diagram.
     */
    public void printSkipList()
    {
        int key;
        Node currentNode = head;
        Level levels[];

        System.out.printf("\n");
        while (currentNode != null)
        {
            levels = currentNode.getLevels();

            if ((key = currentNode.getKey()) < 10)	System.out.printf("%d  ", key);
            else if (key < 100)	                    System.out.printf("%d ",  key);
            else                                	System.out.printf("%d",   key);
            for (Level ignored : levels)            System.out.printf(" | ++ |");

            currentNode = currentNode.getSuccessor();
            System.out.printf("\n");
        }
    }

    public static void main(String[] args)
    {
        String line = null;
        String[] lines;
    	int[] keys;

    	try {
    		BufferedReader input = new BufferedReader(new InputStreamReader(System.in));

    		System.out.printf("Input your numbers, each separated by a space.\nNote: there is no error checking so please enter in ascending order and between 0 - 999\n");
    		line  = input.readLine();
    	} catch (IOException err) { System.exit(1); }

        assert line != null;
        lines = line.split(" ");
        keys  = new int[lines.length];

        for (int i = 0; i < lines.length; i++)	keys[i] = Integer.parseInt(lines[i]);

        SkipList skipList = new SkipList(keys);
        skipList.printSkipList();
    }
}