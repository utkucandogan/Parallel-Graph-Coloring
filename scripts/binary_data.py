import os

def create_binary_data_from_file(output_filename: str, input_filename: str, count: int):
    # Must be multiple of 8
    WORD_LEN = 32
    WORD_LEN_BYTES = WORD_LEN // 8

    ENDIAN = "little"

    input  = open(input_filename, "r")
    output = open(output_filename, "wb")

    # write the size as 32-bit integer
    output.write(count.to_bytes(WORD_LEN_BYTES, ENDIAN))

    for row in range(count):
        # Read the row data
        rowdata = input.readline()

        # Remove the whitespaces
        bit_string = "".join(rowdata.split())

        # Ensure the length of the bit string is a multiple of WORD_LEN by padding with zeros if necessary
        bit_string = bit_string + '0' * (-len(bit_string) % WORD_LEN)

        # List to store the resulting WORD_LEN-bit integers
        int_list = []

        # Process the string in chunks of WORD_LEN bits
        for i in range(0, len(bit_string), WORD_LEN):
            # Get the next WORD_LEN-bit chunk, bits are reversed
            bit_chunk = bit_string[i:i+WORD_LEN][::-1]

            # Convert the bit chunk to a WORD_LEN-bit integer and add to the list
            int_list.append(int(bit_chunk, 2)) # Convert from binary to integer

        # Write the result to the file
        for i in int_list:
            output.write(i.to_bytes(WORD_LEN_BYTES, ENDIAN))

    output.close()
    input.close()

if __name__ == "__main__":
    path = os.path.dirname(__file__)

    create_binary_data_from_file(os.path.join(path, "out/sudoku.dat"), os.path.join(path, "data/sudoku.txt"), 81)
