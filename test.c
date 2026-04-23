int g;
void test(char	**arg){
  if (**arg != ')')
	{
	    g++;
	}
  ++*arg;
  g++;
}
