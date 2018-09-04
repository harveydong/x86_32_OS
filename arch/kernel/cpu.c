void set_cpu_online(unsigned int cpu,bool online)
{
	if(online)
		cpumask_set_cpu(cpu,to_cpumask(cpu_online_bits));

	else
		cpumask_clear_cpu(cpu,to_cpumask(cpu_online_bits));
}
