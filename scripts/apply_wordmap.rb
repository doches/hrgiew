#!/usr/bin/env ruby
#
# Apply a wordmap to all files with a given prefix
#
# Usage: apply_wordmap path/to/wordmap prefix/files

if ARGV.size != 2
    STDERR.puts "Apply a wordmap to all files with a given prefix"
    STDERR.puts ""
    STDERR.puts "Usage: #{$0} path/to/wordmap prefix/files"
    exit(0)
end

wordmap_path = ARGV.shift
prefix = ARGV.shift

def process(file)
	ext = file.split(".").pop
	
	txt = IO.readlines(file).join("").strip
	case ext
		when "dot"
			@wordmap.each_pair { |index,word| txt.gsub!("\"#{index}\"","\"#{word}\"") if not word.nil? }
			return txt
		when "matrix"
			@wordmap.each_pair { |index,word| txt.gsub!(/^#{index}\:/,"#{word}:") if not word.nil? }
			return txt
		when "txt"
			@wordmap.each_pair { |index,word| txt.gsub!("<#{index}>",word) if not word.nil? }
			return txt
		when "graph"
			@wordmap.each_pair { |index,word| txt.gsub!(/^#{index}\t/,"#{word}\t") if not word.nil? }
			@wordmap.each_pair { |index,word| txt.gsub!(/\t#{index}\t/,"\t#{word}\t") if not word.nil? }
			return txt
		else
			STDERR.puts "Unrecognised file extension \"#{ext}\""
			return nil
	end
end

@wordmap = {}
IO.foreach(wordmap_path) do |line|
	word,index = *(line.strip.split(/\s+/))
	index = index.to_i
	@wordmap[index] = word if @wordmap[index].nil?
end

Dir.glob("#{prefix}*").each do |file|
	txt = process(file)
	if not txt.nil?
		new_filename = file.split(".").reject { |x| x == "human" }
		ext = new_filename.pop
		new_filename = new_filename.push("human").push(ext).join(".")
		fout = File.open(new_filename,'w')
		fout.puts txt
		fout.close
	end
end
