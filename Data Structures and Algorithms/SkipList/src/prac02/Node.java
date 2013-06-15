package prac02;

public class Node
{
	private Level[] levelArr;
	private Node successor;
	private Node predecessor;
	private int height;
	private int key;
    private int levelNum;

	protected class Level
	{
		protected Node next;
		protected Node prev;

		protected Level()
		{
			next = null;
			prev = null;
		}

		private void setLevelPointers(Node initPrev, Node initNext)
		{
			prev = initPrev;
			next = initNext;
		}
	}

	public Node(int initKey)
	{
		int i;
		height = 1;			// height needs to be >= 1
		while (Math.random() < 0.5)
			height++;

		key = initKey;
        levelNum = height -1;
		levelArr = new Level[height];
		for (i = 0; i < height; i++)
			levelArr[i] = new Level();

	}

	public void fillNode()
	{
		Node next;
		Node prev;
		boolean found;

		for (int i = 0; i < this.height; i++)
		{
            prev = this.predecessor;
			found = false;
			while (prev != null && !found)
			{
				if (prev.height > i)    // prev Node is taller than given story
					found = true;
				else
					prev = prev.predecessor;
			}

            next = this.successor;
		    found = false;
			while (next != null && !found)
			{
				if (next.height > i)	// next Node is taller than given story
					found = true;
				else
					next = next.successor;
			}

			levelArr[i].setLevelPointers(prev, next);
		}
	}

	public boolean descend()
	{
		if (levelNum > 0)
		{
            levelNum--;
			return true;
		} else
			return false;
	}

	public int getKey()
	{
		return key;
	}

    public int getHeight()
    {
        return height;
    }

    public Level[] getLevels()
    {
        return levelArr;
    }

    public Node getPredecessor()
    {
        return predecessor;
    }

    public Node getSuccessor()
    {
        return successor;
    }

    public void setNodePointers(Node initPredecessor, Node initSuccessor)
    {
    	predecessor = initPredecessor;
    	successor = initSuccessor;
    }
}
