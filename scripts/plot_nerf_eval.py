#! /usr/bin/env python
"""Generates plot data for Nerf skeleton evaluation.
"""

import collections
import math
import json
import sys


def compute_pdf_for_joint_distances(joint_distances):
    """Takes in a parsed distances.json and outputs the threshold counts and
    the number of data points. Note that points out of range may be skipped.
    """
    num_points = 0
    pdf = [0 for i in range(100)]
    max_distance = -1
    num_negatives = 0

    for joint_name, distances in joint_distances.items():
        for distance in distances:
            if distance < 0:
                num_negatives += 1
                continue
            num_points += 1
            index = int(math.ceil(distance * 100))
            pdf[index] += 1
        max_joint_distance = max(distances)
        max_distance = max(max_distance, max_joint_distance)
    return pdf, num_points, max_distance


def main():
    if len(sys.argv) < 2:
        print 'Usage: ./plot_nerf_eval.py DISTANCES1.json DISTANCES2.json ...'
        return

    # pdf gives a count of data points within a certain range
    # pdf[0] is for distances x such that 0 <= x < 0.01 and
    # pdf[20] is for distances such that 0.20 <= x < 0.21.
    pdf = [0 for i in range(100)]
    counters = collections.Counter()

    num_points = 0
    max_distance = -1

    for file_i in range(1, len(sys.argv)):
        distances_file = open(sys.argv[file_i])
        joint_distances = json.load(distances_file)
        file_pdf, file_pts, file_max = compute_pdf_for_joint_distances(joint_distances)
        for i in range(100):
            pdf[i] += file_pdf[i]
        num_points += file_pts
        max_distance = max(max_distance, file_max)
    max_index = int(math.ceil(max_distance * 100)) + 1
    max_index = 21

    pdf = [float(x) / num_points for x in pdf]

    cdf = [1 for i in range(max_index)]
    cdf[0] = pdf[0]
    auc = 0
    for i in range(1, max_index):
        cdf[i] = cdf[i - 1] + pdf[i]
        auc += (cdf[i - 1] + cdf[i]) / 2 * 0.01
    total_area = (max_index-1) / 100.0
    auc /= total_area

    x = [0.01 * i for i in range(max_index)]
    for i, val in enumerate(cdf):
        print '{},{}'.format(x[i], val)
    print 'AUC: {}, Total points: {}'.format(auc, num_points)


if __name__ == '__main__':
    main()
