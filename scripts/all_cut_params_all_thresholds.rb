# Scan `input/` for humanised dotfiles and determine the best cut
# threshold for each.
#
# Usage: ruby #{$0} input_dir output_dir

input = ARGV.shift
output = ARGV.shift

Dir.glob("#{input}/*.consensus.human.dot").map { |x| `ruby scripts/eval_cut_param.rb #{x} subset.yaml > #{output}/#{x.split('/')[1].split('.')[0]}.dat` }