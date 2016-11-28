#!/usr/bin/python3
import json
import argparse
import numpy

def get_data(path):
    with open(path) as file:
        data = json.load(file)
        return data

def main():
    parser = argparse.ArgumentParser(description="Deadlock Detection")
    parser.add_argument("filename", help="Json file with sample data")
    args = parser.parse_args()

    data = get_data(args.filename)

    p = data["processes"]
    r = data["resources"]
    allocation = numpy.array(data["allocation"])
    available = numpy.array(data["available"])
    request = numpy.array(data["request"])

    finish = numpy.zeros(p)
    deadlock = numpy.zeros(r)

    progress = True
    while not all(finish) or progress:
        progress = False
        for i in range(p):
            # check if process is already finished
            if finish[i]:
                continue
            print("av: {} req[{}]: {}".format(available, i, request[i]))
            # are there enough resources for the request?
            if all(request[i] <= available):
                print("valid")
                progress = True
                # allocate resources
                available = available - request[i]
                print("allocated av: {}".format(available))
                # release resources
                available = available + allocation[i]
                print("released av: {}".format(available))
                # mark process as finished
                finish[i] = 1
                break

    if all(finish):
        print("safe state")
    else:
        print("unsafe state")

    return 0

if __name__ == "__main__":
    print("\nMain exited with: {}".format(main()))
