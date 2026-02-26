import pandas
import matplotlib.pyplot
import numpy

def draw_partitioned_graph(df: pandas.DataFrame, x_label: str, y_label: str, z_label: str, output_filepath: str):
  Zs = numpy.unique(df[z_label])
  x_labels = sorted(numpy.unique(df[x_label]))
  matplotlib.pyplot.figure(figsize=(20, 20))
  for z in Zs:
    subdf = df[df[z_label] == z]
    Xs = sorted(numpy.unique(subdf[x_label]))
    Ys = [numpy.mean(subdf[subdf[x_label] == x][y_label]) for x in x_labels]
    matplotlib.pyplot.plot(Xs, Ys, label=z)
  matplotlib.pyplot.legend()
  matplotlib.pyplot.title('%s given %s and partitioned by %s' % (y_label, x_label, z_label))
  matplotlib.pyplot.savefig(output_filepath)
  matplotlib.pyplot.close()

def main():
  df = pandas.read_csv('./out.scale.csv')
  draw_partitioned_graph(df, 'length_text', 'comparison_count', 'length_alphabet', './out.comparisons.png')
  draw_partitioned_graph(df, 'length_text', 'interval_count', 'length_alphabet', './out.intervals.png')
  draw_partitioned_graph(df, 'length_text', 'elapsed_time', 'length_alphabet', './out.time.png')
  draw_partitioned_graph(df, 'length_text', 'palindrome_length', 'length_alphabet', './out.palindrome_length.png')
  draw_partitioned_graph(df, 'length_text', 'palindrome_number', 'length_alphabet', './out.palindrome_number.png')

if __name__ == '__main__':
  main()
