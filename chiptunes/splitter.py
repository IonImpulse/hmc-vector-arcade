with open("trans.dmp", "rb") as f:
    data = f.read()

    # Split into chunks of 48 bytes
    chunk_size = 25
    chunks = [data[i:i+chunk_size] for i in range(0, len(data), chunk_size)]

    # Each chunk is two hex numbers, so we need to split them into two bytes

    for (i, chunk) in enumerate(chunks):
        print(f"Chunk {i}: {chunk.hex(' ')}")