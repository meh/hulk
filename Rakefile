require 'rake'
require 'rake/clean'

CC      = 'clang'
CFLAGS  = "-Wall -Werror-implicit-function-declaration -Wno-format -std=c11 -Iinclude #{ENV['CFLAGS']}"
LDFLAGS = "#{ENV['LDFLAGS']}"

SOURCES = FileList['source/**/*.c']
OBJECTS = SOURCES.ext('o')

CLEAN.include(OBJECTS)
CLOBBER.include('hulk')

task :default => 'hulk'

file 'hulk' => OBJECTS do
	sh "#{CC} -o hulk #{OBJECTS} #{LDFLAGS}"
end

task :image do
	sh 'mkdir -p test'
	sh 'dd if=/dev/zero of=test.img bs=1048576 count=10'
	sh 'mkfs.ext4 -F test.img'
	sh 'sudo mount -o loop test.img test'
	sh 'dd if=/dev/urandom of=test/a bs=1048576 count=1'
	sh 'dd if=/dev/urandom of=test/b bs=1048576 count=3'
	sh 'mkdir -p test/c/d/e'
	sh 'dd if=/dev/urandom of=test/c/d/e/f bs=1048576 count=4'
	sh 'sudo umount test'
	sh 'rmdir test'
end

rule '.o' => '.c' do |t|
	sh "#{CC} #{CFLAGS} -o #{t.name} -c #{t.source}"
end
