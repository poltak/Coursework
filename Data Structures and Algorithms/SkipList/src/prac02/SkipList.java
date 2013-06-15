package prac02;
import java.io.*;
import prac02.Node.Level;

public class SkipList
{
	private int noOfNodes;
    private Node head;
    private Node[] nodeArr;

	public SkipList(int[] keys)
	{
        int i;
        noOfNodes = keys.length;
        nodeArr = new Node[noOfNodes];

        //int[] iArr = {1,3,4,6,7,8,9};       // for hard coded list to test

        for (i = 0; i < noOfNodes; i++)
        	nodeArr[i] = new Node(keys[i]);


        setNodeLinks();

        for (i = 0; i < noOfNodes; i++)
        	nodeArr[i].fillNode();

        head = nodeArr[0];
	}

	private void setNodeLinks()
	{
        Node next, prev;

	    for (int i = 0; i < noOfNodes; i++)
        {
            if (i-1 < 0)            // if first Node
                prev = null;
            else
                prev = nodeArr[i-1];

            if (i+1 >= noOfNodes)   // if last Node
                next = null;
            else
                next = nodeArr[i+1];

            nodeArr[i].setNodePointers(prev, next);
        }
	}

	public boolean search(int searchKey)
	{
		if (searchKey < 0 || searchKey > 999 || search(searchKey, head).getKey() != searchKey)
			return false;
		return true;
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
        if (currentNode.getKey() <= searchKey)		// don't print path of keys larger than searchKey
        	System.out.printf("%d\n", currentNode.getKey());

        if (currentNode.getKey() == searchKey)
            return currentNode;
        else {
            while (levelArr[levelCounter].next == null || levelArr[levelCounter].next.getKey() > searchKey)
                if (levelCounter != 0)
                	levelCounter--;
                else
                    return currentNode;

            return search(searchKey, levelArr[levelCounter].next);
        }
    }

    public boolean delete(int deleteKey)
    {
    	Node deleteNode = search(deleteKey, head);

    	if (deleteNode.getKey() != deleteKey)
    		return false;
    	delete(deleteKey, deleteNode);
        return true;
    }

    private void delete(int deleteKey, Node deleteNode)
    {
        Node currentLeft = deleteNode.getPredecessor();
        Node currentRight = deleteNode.getSuccessor();

        // sort out left links
        if (currentLeft != null) {
            currentLeft.setNodePointers(currentLeft.getPredecessor(), deleteNode.getSuccessor());
            currentLeft.fillNode();
        } else {		// if we're deleting left-most Node
        	currentRight.setNodePointers(null, currentRight.getSuccessor());
        	currentRight.fillNode();
        	head = head.getSuccessor();
        }
        // sort out right links
        if (currentRight != null) {
            currentRight.setNodePointers(deleteNode.getPredecessor(), currentRight.getSuccessor());
            currentRight.fillNode();
        } else {     	// if we're deleting right-most Node
        	currentLeft.setNodePointers(currentLeft.getPredecessor(), null);
        	currentLeft.fillNode();
        }

        printSkipList();
    }

    public boolean insert(int insertKey)
    {
    	Node searchResult = search(insertKey, head);

    	if (searchResult.getKey() == insertKey)
    		return false;

    	insert(insertKey, searchResult.getPredecessor());
    	return true;
    }

    @SuppressWarnings("unused")
	private void insert(int insertKey, Node prevNode)
    {
    	int i = getNextEmptyIndex();
    	Node nextNode = prevNode.getSuccessor();
    	Node predecessor;

    	nodeArr[i] = new Node(insertKey);
    	if (prevNode != null) {
    		prevNode.setNodePointers(prevNode.getPredecessor(), nodeArr[i]);
    		predecessor = prevNode;
    	} else {
    		predecessor = null;
    	}

    	if (nextNode != null) {
    		nextNode.setNodePointers(nodeArr[i], nextNode.getSuccessor());

    	} else {

    	}

    }

    public int getNextEmptyIndex()
    {
        for (int i = 0; i < nodeArr.length; i++)
        	if (nodeArr[i] == null)
        		return i;

        return -1;
    }

    public static void main(String[] args)
    {
//        String line;
//        String[] lines;
//    	int i;
//    	int[] keys;
//
//    	try {
//    		BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
//
//    		System.out.printf("Input your numbers, each separated by a space.\nNote: there is no error checking so please enter in ascending order and between 0 - 999\n");
//    		line = input.readLine();
//    		lines = line.split(" ");
//    		keys = new int[lines.length];
//
//    		for (i = 0; i < lines.length; i++)
//    			keys [i] = Integer.parseInt(lines[i]);
//
//    		if (delete(5))
//    			System.out.printf("Yes\n\n");
//    		else
//    			System.out.printf("No\n\n");
//    	} catch (IOException err) {
//    		System.err.printf("Derp.\n");
//    	} catch (NumberFormatException err) {
//    		System.err.printf("Derp.\n");
//    	}

    }

    public void printSkipList()
    {
    	int i, key;
        Node currentNode = head;
    	Level levelArr[];

        System.out.printf("\n");
        while (currentNode != null)
        {
            levelArr = currentNode.getLevels();

            if ((key = currentNode.getKey()) < 10)
    			System.out.printf("%d  ", key);
    		else if (key < 100)
    			System.out.printf("%d ", key);
    		else
    			System.out.printf("%d", key);

            for (i = 0; i < levelArr.length; i++)
                System.out.printf(" | ++ |");

            currentNode = currentNode.getSuccessor();
            System.out.printf("\n");
        }
    }
}
