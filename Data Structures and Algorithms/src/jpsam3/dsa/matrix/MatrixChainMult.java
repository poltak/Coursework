package jpsam3.dsa.matrix;
/*
 *  Name:           Jonathan Poltak Samosir
 *  Email:          jpsam3@student.monash.edu
 *  Date:           2012-10-15
 */

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class MatrixChainMult
{
	private int[] matrixDimensions;

    /**
     * Reads the specified file and hands each line over to handleLine().
     * @param filePath Path to data file.
     */
	public void handleFile(String filePath) throws RuntimeException
    {
		try {
			BufferedReader in = new BufferedReader(new FileReader(filePath));

            String line;
			while ((line = in.readLine()) != null)	// Extract data from each line in file.
				handleLine(line);

			in.close();
		} catch (IOException e) {
			System.err.println("Fatal IO error occurred:");
            e.printStackTrace();
            throw new RuntimeException();
		}
	}

    /**
     * Converts the line to a number of integers that will be filled into matrixDimensions[].
     * @param line Line to extract data from.
     */
	private void handleLine(String line)
	{
		int i = 0;
		String[] dimensions = line.split(" ");

		this.matrixDimensions = new int[dimensions.length];
		for (String currentDimension : dimensions)
			matrixDimensions[i++] = Integer.parseInt(currentDimension);
	}

	/**
	 * Brain overheated trying to understand this; consult in-line comments for easier understanding.
	 *
	 * Should be O(n^3) time algorithm, which may seem to take more time than the non-dynamic programming version but
	 * given the constants involved it's faster most of the time to do the calculations to find out the optimal
	 * multiplication ordering than performing the actual matrix multiplications.
	 */
	public void getOptimalMults()
	{
		int inputLen = matrixDimensions.length -1;					// No. of ints in the input file.
		int[][] multTable = new int[inputLen][inputLen];			// Create a table to store multiplication products
		                                                            // of specified matrices, i.e.:
		                                                            // multTable[i][n] = # of mults in Ai*Ai+1*...*An
		for (int i = 0; i < inputLen; i++)
			multTable[i][i] = 0;

		for (int chainLen = 1; chainLen <= inputLen; chainLen++)	// For each matrix chain variation.
		{
			for (int left = 0; left < (inputLen-chainLen); left++)	// For each position that is relevant to current variation of matrix chain (there will be inputLen-chainLen indices to work with in this subsection).
			{
				int right = left + chainLen;						// To ensure right index actually starts from the right most value of left.
				multTable[left][right] = Integer.MAX_VALUE;			// Set the value of the current index to the max value as we can then update it to the min value we get after calculating.

				for (int i = left; i < right; i++)					// For each position between left and right.
				{
                    // Our actual calculation to work out the given no. of multiplications for the given matrices.
					int noOfMults = multTable[left][i] + multTable[i+1][right] + matrixDimensions[left]
                            * matrixDimensions[i+1] * matrixDimensions[right+1];

					if (noOfMults < multTable[left][right])			// Only add this calculated value to the table if it is more optimal than the previously stored value.
						multTable[left][right] = noOfMults;
				}
			}
		}
		printMostOptimal(multTable);
	}

    /**
     * Prints out most optimal amount of multiplications using the table of multiplications.
     * @param table Table to print the values for.
     */
	private void printMostOptimal(int[][] table)
	{
		int firstRow = 0;
		int lastCol  = table.length -1;
        // Print out value in first row, last column (due to the way the table is computed, this will be the min amount
        // of multiplications).
		System.out.printf("Minimum amount of multiplications: %d%n", table[firstRow][lastCol]);
	}

	public static void main(String[] args)
	{
		MatrixChainMult test = new MatrixChainMult();

		test.handleFile("jpsam3/dsa/matrix/matrix.txt");
		test.getOptimalMults();
	}
}
