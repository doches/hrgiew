# Runs dendrify, resample, consensus, apply_wordmap, and eval | regress for a 
# number of candidate values for the threshold parameter
#
# Usage: ruby #{$0} path/to/handle.[graph|wordmap] count p1 p2 p3 ...

handle = ARGV.shift

count = ARGV.shift.to_i

if handle =~ /\.(graph|wordmap)$/
	h = handle.split(".")
	h.pop
	handle = h.join(".")
end

dir = File.dirname(handle)

scorepath = "#{dir}/scores.yaml"

scorefile = {}
scorefile = YAML.load_file(scorepath) if File.exists?(scorepath)

threads = []
ARGV.map { |x| x.to_f }.each do |threshold|
	threads << Thread.new(threshold,handle,dir) do |t,corpus,d|
		(0..count-1).each do |n|
		
		t_h = "#{t}".gsub('.','_')
		output_dir = d
		init_dendrogram = "#{corpus}.#{n}.#{t_h}.init.dendrogram"
		max_sample = 100000
		
		if not File.exists?(init_dendrogram)
			puts "dendrify #{t}"
			`./dendrify #{corpus}.graph #{t} > #{init_dendrogram}`
		end
		
		if not File.exists?("#{output_dir}/#{n}.#{t_h}.dendrogram")
			puts "resample #{t}"
			`./resample #{corpus}.graph #{init_dendrogram} #{output_dir}/ #{n}.#{t_h} 1000 #{t} #{max_sample}`
		end
		if not File.exists?("#{output_dir}/#{n}.#{t_h}.consensus.txt")
			puts "consensus #{t}"
			cmd = "./consensus #{corpus}.graph #{output_dir}/#{n}.#{t_h}.dendrogram 2000 10 #{output_dir}/#{n}.#{t_h} #{t}"
			`#{cmd}`
		end
		
		puts "wordmap #{t}"
		`/usr/bin/ruby apply_wordmap #{corpus}.wordmap #{output_dir}/#{n}.#{t_h} 2> /dev/null`
		
		correlation = "#{output_dir}/#{n}.#{t_h}.correlation"
		if not File.exists?(correlation)
			puts "score #{t}"
			score = `/usr/bin/ruby eval #{output_dir}/#{n}.#{t_h}.consensus.human.matrix wordnet.matrix 2> /dev/null | regress`.split("\n").pop.split("\s+").shift.to_f
			`echo \"#{score}\" > #{correlation}`
			scorefile[t] = score
		end
	end
	
	end
end
threads.each { |t| t.join }
puts `/usr/bin/ruby ./render #{dir}`

fout = File.open(scorepath,'w')
fout.puts scorefile.to_yaml
fout.close
