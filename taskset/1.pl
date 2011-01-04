#!/usr/bin/perl
system('./matrS.exe lala');
system('./matrS.exe lala &');
$a = `ps -A`;
@a = split('\n',$a);
for(@a)
{
    if($_=~/matrS/)
    {
	@tmp = split(' ',$_);
	$pid = @tmp[0];
	print $pid;
    }
}
$proc;
for(1.20)
{
    if($_%2)
    {
	$proc = 1;
    }
    else
    {
	$proc = 2;
    }
    system("taskset $proc -p $pid");
    sleep2;
    
}
