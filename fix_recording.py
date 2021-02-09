import argparse
import loris

parser = argparse.ArgumentParser(description='Fix tcommon issues in recorded files, and mnormalize the timestamps')
parser.add_argument('input', type=str, help='the input .es file')
parser.add_argument('output', type=str, help='the output .es file')
arguments = parser.parse_args()

if arguments.input == arguments.output:
    raise Exception('the input and output files must be different')

stream = loris.read_file(arguments.input)
if len(stream['events']) > 0:
    previous_t  = stream['events'][0][0]
    for index in range(1, 1000):
        t = stream['events'][index][0]
        if t - previous_t > 1000000:
            stream['events'] = stream['events'][index:]
            break
        previous_t = t
    stream['events']['ts'] -= stream['events'][0][0]
loris.write_events_to_file(stream, arguments.output)
