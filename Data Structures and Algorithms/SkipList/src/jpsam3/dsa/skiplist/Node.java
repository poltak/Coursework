package jpsam3.dsa.skiplist;

public class Node
{
	private Level[] levels;
	private Node    predecessor, successor;
	private int     height, key, levelIndex;

	public Node(int initKey)
	{
		do ++height;    while (Math.random() < 0.5);    // Get pseudo-random height for each Node.
		key       = initKey;
        levelIndex = height -1;
		levels    = new Level[height];

        // Initialise all Levels.
		for (int i = 0; i < levels.length; i++)     levels[i] = new Level();
	}

	public void fillNode()
	{
		Node next, prev;
		boolean found = false;

		for (int i = 0; i < this.height; i++)
		{
            prev = this.predecessor;
			while (prev != null && !found)
			{
                // If prev Node is taller than given story.
				if (prev.height > i)	found = true;
				else	                prev  = prev.predecessor;
			}

            next = this.successor;
			while (next != null && !found)
			{
                // If next Node is taller than given story.
				if (next.height > i)	found = true;
				else	                next  = next.successor;
			}

			levels[i].setLevelPointers(prev, next);
		}
	}

	public boolean descend()
	{
		if (levelIndex > 0)
		{
            levelIndex--;
			return true;
		}
		return false;
	}

    public void setNodePointers(Node initPredecessor, Node initSuccessor)
    {
        predecessor = initPredecessor;
        successor   = initSuccessor;
    }

    public Level[]  getLevels()      { return levels;       }
    public Node     getPredecessor() { return predecessor;  }
    public Node     getSuccessor()   { return successor;    }
	public int      getKey()         { return key;          }
    public int      getHeight()      { return height;       }

    /**
     * Inner class used to represent the individual levels within a SkipList Node.
     */
    protected class Level
    {
        protected Node next, prev;

        protected Level()   { next = prev = null; }

        private void setLevelPointers(Node initPrev, Node initNext)
        {
            prev = initPrev;
            next = initNext;
        }
    }
}
