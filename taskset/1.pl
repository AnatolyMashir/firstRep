#!/usr/bin/perl
use Time::HiRes qw(time usleep); 
system('./matrS.exe lala &');
$a = `ps -A`;
@a = split('\n',$a);
for(@a)
{
    if($_=~/matrS/)
    {
	@tmp = split(' ',$_);
	$pid = @tmp[0];
	print "pid=$pid\r\n";
    }
}
$proc=0;
while(!system("taskset -pc $proc $pid"))
{
    $proc++;
    $proc=$proc%2;
#    sleep 1;
    usleep 100;
}
print "One proc\r\n";
system('./matrS.exe lala');