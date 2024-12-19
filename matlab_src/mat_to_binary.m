matfile=load("/MATLAB Drive/494_bus.mat");
data_array=(matfile.Problem.A);
nonzero = spones(matfile.Problem.A);
matrix_length=(length(data_array));

% Set the Lower Triangle and Diagonal to Zero (excluding upper triangle)
for i = 1:matrix_length
    for j = 1:i  % Zero out the diagonal and lower triangle
        nonzero(i, j) = 0;
    end
end
nonzero = nonzero + nonzero.';
nnz_per_row = sum(nonzero ~= 0, 2); % Sum along columns (2 means row-wise)

% Find the row with the most nonzero elements
[max_nnz, max_row] = max(nnz_per_row)
[rowSize, colSize] = size(nonzero)


% Find Nonzero Elements and Their Positions
[rowIndices, colIndices] = find(nonzero);
%scatter(colIndices,rowIndices)

% Combine Row and Column Indices into a Single Value
linearIndices = (colIndices - 1) * colSize + (rowIndices-1); % 0-based indexing

% Save Linear Indices to a Binary File
fileName = sprintf('matrix_%dx%d.bin', rowSize, colSize);
fileID = fopen(fileName, 'wb'); % Open file for writing in binary mode
[i,j,k] = find(max_nnz);
element = uint32(k);
% Write Linear Indices
fwrite(fileID, matrix_length, 'uint32');
fwrite(fileID, element, 'uint32');
fwrite(fileID, linearIndices, 'uint32');

fclose(fileID);

disp(['Compact nonzero positions saved to ', fileName]);