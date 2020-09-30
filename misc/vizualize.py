import argparse
import imageio
import os

import matplotlib.pyplot as plt
import numpy as np

CWD = os.getcwd()

def make_args_parser():
    # create an ArgumentParser object
    parser = argparse.ArgumentParser()
    # fill parser with information about program arguments
    parser.add_argument('-o', '--output_file',
                        default=CWD + '/outputs/small',
                        help='Define the output file to create its gif')
    parser.add_argument('-d', '--dir', default=CWD + '/gifs/',
                        help='Define the output dir for generated gif')
    parser.add_argument('-s', '--size', default=8, help='Define the size of the grid')
    # return an ArgumentParser object
    return parser.parse_args()

def print_args(args):
    print("Running with the following configuration")
    # get the __dict__ attribute of args using vars() function
    args_map = vars(args)
    for key in args_map:
        print('\t', key, '-->', args_map[key])
    # add one more empty line for better output
    print()

def main():
    # parse and print arguments
    args = make_args_parser()
    print_args(args)

    grid = np.zeros((int(args.size), int(args.size)))

    with open(args.output_file) as f:
        # For each generation (aka file line)
        for cnt, line in enumerate(f):
            row = 0
            col = 0
            # Fill grid
            for i, c in enumerate(line):
                # Surpass '\n' at the end of the line
                if i == int(args.size) * int(args.size):
                    break
                # Otherwise calculate coordinates and fill the grid
                if i % int(args.size) == 0 and i != 0:
                    row += 1
                    col = 0
                grid[row][col] = c
                col += 1
            # Plot grid and save its image
            plt.title("Generation " + str(cnt))
            plt.imshow(grid, cmap='Greys',  interpolation='nearest')
            image_name = args.dir + str(cnt) + '.png'
            plt.savefig(image_name)

    # Create gif
    images = []
    for file_name in os.listdir(args.dir):
        if file_name.endswith('.png'):
            file_path = os.path.join(args.dir, file_name)
            images.append(imageio.imread(file_path))
            os.remove(file_path)
    path = '/'.join(args.output_file.split('/')[:-1]) + '/gifs/' + args.output_file.split('/')[-1] + '.gif'
    imageio.mimsave(path, images)

if __name__ == '__main__':
    main()
