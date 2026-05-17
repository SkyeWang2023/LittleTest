
int g = 0;
void test(char	**arg){
  if(arg && *arg){
    if (**arg != ')')
      {
          g++;
      }
    ++*arg;
  }
 
}

