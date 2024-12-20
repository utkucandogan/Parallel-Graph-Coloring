import os

def create_binary_data_from_file(output_filename: str, input_filename: str, count: int):
    WORD_LEN = 4
    ENDIAN = "little"

    input  = open(input_filename, "r")
    output = open(output_filename, "wb")

    # write the size as 32-bit integer
    output.write(count.to_bytes(WORD_LEN, ENDIAN))

    # write the degree temporarily
    max_degree = 0
    output.write(max_degree.to_bytes(WORD_LEN, ENDIAN))

    for row in range(count):
        # Read the row data
        row_data = input.readline()
        neighbors = [int(n) for n in row_data.split()]

        degree = 0

        for col, n in enumerate(neighbors):
            if n:
                degree += 1
                data = row * count + col
                output.write(data.to_bytes(WORD_LEN, ENDIAN))

        if degree > max_degree:
            max_degree = degree

    # update the degree
    output.seek(WORD_LEN)
    output.write(max_degree.to_bytes(WORD_LEN, ENDIAN))

    output.close()
    input.close()

if __name__ == "__main__":
    path = os.path.dirname(__file__)

    create_binary_data_from_file(os.path.join(path, "out/sudoku.bin"), os.path.join(path, "data/sudoku.txt"), 81)
