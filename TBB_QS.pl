use Cwd;
$cur_dir = getcwd;
	
open(W, '>' , 'result_TBB_QS.txt');
$SNL = $ENV{'SLURM_NODELIST'}; 

$nodelist = `scontrol show hostname $SNL|paste -d, -s`;
chomp($nodelist);

for($i = 1000; $i < 10000000000; $i*=10)
{
  $result=`mpiexec -n 1 -ppn 1  -hosts $nodelist ./TBB_QS $i`;
  print W "$i ".$result;
}

close W;