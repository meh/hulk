require 'rake'
require 'rake/clean'

CC      = 'clang'
CFLAGS  = "-Wall -Werror-implicit-function-declaration -std=c11 -Iinclude #{ENV['CFLAGS']}"
LDFLAGS = "#{ENV['LDFLAGS']}"

SOURCES = FileList['source/**/*.c']
OBJECTS = SOURCES.ext('o')

CLEAN.include(OBJECTS)
CLOBBER.include('hulk')

task :default => 'hulk'

file 'hulk' => OBJECTS do
	sh "#{CC} -o hulk #{OBJECTS} #{LDFLAGS}"
end

rule '.o' => '.c' do |t|
	sh "#{CC} #{CFLAGS} -o #{t.name} -c #{t.source}"
end
