# Finding Short Cycles in Private Networks

This is the code I used to test the algorithm as described in my thesis[^1].
Paper is WIP.

## Testing Data

The main data for testing was generated using the Barabási–Albert model[^2].
The raw data used for some of the testing was taken from a synthetic data set from IBM[^3][^4].
To start, we drop every column besides the source and target accounts.
We then factorise the account values in the dataset to integers instead of hexadecimal account numbers.
After this, we filter the graph to remove duplicate edges and self-loops.

[^1]: J. Jense, *Interactive Structuring and Vectorization of Sketch Imagery*, M.Sc. thesis, Faculty of Electrical Engineering, Mathematics and Computer Science, Delft University of Technology, Delft, Netherlands, 2024. [Online](https://resolver.tudelft.nl/uuid:a559439a-d5ee-4f1f-9823-2bab3905c0c9)

[^2]: R. Albert and A.-L. Barabási, "Statistical mechanics of complex networks," *Reviews of Modern Physics*, vol. 74, no. 1, pp. 47–97, 2002. doi: [10.1103/RevModPhys.74.47](https://doi.org/10.1103/RevModPhys.74.47)

[^3]: E. Altman, J. Blanusa, L. Von Niederhäusern, B. Egressy, A. S. Anghel, and K. Atasu, *IBM Transactions for Anti Money Laundering (AML)*, Kaggle, 2023. [Online](https://www.kaggle.com/datasets/ealtman2019/ibm-transactions-for-anti-money-laundering-aml)

[^4]: E. Altman, J. Blanusa, L. Von Niederhäusern, B. Egressy, A. S. Anghel, and K. Atasu, "Realistic Synthetic Financial Transactions for Anti-Money Laundering Models," *arXiv preprint arXiv:2306.16424*, Jan. 2024. [Online](https://arxiv.org/abs/2306.16424)
