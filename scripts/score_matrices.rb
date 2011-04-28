# Takes two matrices and computes a score for how similar they are.

def file_to_matrix(filename)
    matrix = {}
    indices = {}
    line_index = 0
    dims = 0
    IO.foreach(filename) do |line|
        cols = line.split(/\t/)
        key = cols.shift.gsub(":",'')
        indices[line_index] = key
        matrix[key] = cols.map { |x| x.to_i }
        line_index += 1
    end
    
    matrix.each_pair do |key,row| 
        matrix[key] = {}

        row.each_with_index { |value,index| matrix[key][indices[index]] = value }
        #        matrix[key] = matrix[key].map { |k,v| [k,v] }.sort { |a,b| a[0] <=> b[0] }
    end
    
    return matrix
end

candidate = file_to_matrix(ARGV.shift)
gold = file_to_matrix(ARGV.shift)

c_keys = candidate.keys.sort

c_keys.each_with_index do |i,i_index|
    c_keys[0..i_index-1].each do |j|
        #        puts [i,j].join(" ")
        scores = [[candidate,gold].map { |x| x[j][i] }].flatten
        puts scores.join("\t") if not scores.reject { |x| x.nil? }.empty?
    end if i_index > 0
end