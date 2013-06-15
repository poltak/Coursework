package jpsam3.dsa.disjointsets;

public class DisjointSet
{
	private static int[] set;

	/*
	 * basic constructor to initialise all elements in set to -1
	 */
	public DisjointSet(int size)
	{
		set = new int[size];

		for (int i = 0; i < set.length; i++)
			set[i] = -1;
	}

	/*
	 * performs a union-by-size on subsets root1 and root2
	 */
	public static void union(int root1, int root2)
	{
		if (set[root1] <= set[root2]) {			// if root1 is the larger size tree or both trees are equal
			set[root2] = root1;

			if (set[root2] < 0)					// updates root1's size
				set[root1] += set[root2];
			else
				set[root1]--;
		} else {								// if root2 is the larger size tree
			set[root1] = root2;

			if (set[root1] < 0)					// updates root2's size
				set[root2] -= set[root1];
			else
				set[root2]--;
		}

		printSets();
	}

	/*
	 * prints out the array for the disjoint sets showing connectivities
	 */
	public static void printSets()
	{
		System.out.printf("value:  ");
		for (int i : set)
			System.out.printf("%d  ", i);

		System.out.printf("\n\n");
	}

	public static void main(String[] args)
	{
//		int size = 10;
//		jpsam3.dsa.disjointsets.DisjointSet ds = new jpsam3.dsa.disjointsets.DisjointSet(size);
		union(0,1);
		union(0,2);
		union(0,3);

		union(5,6);
		union(5,7);
		union(5,8);

		union(0,5);
	}
}
