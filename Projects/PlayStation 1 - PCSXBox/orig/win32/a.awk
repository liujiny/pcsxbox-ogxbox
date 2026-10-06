BEGIN {

x = 0 ;

  getline <"c:\\psxlog.txt" ;
  line1 = $1 ;
  ppc1 = substr($2,7) ;
  getline <"c:\\psxlog2.txt" ;
  line2 = $1 ;
  ppc2 = substr($2,7) ;

while ( ( length(line1) > 0 ) && ( length(line2) > 0 ) )
{
x++ ;
	if ( line1 != line2 )
	{
		printf("%s %s %u\r\n", line1, line2,  x );
	}

	if ( ppc1 != ppc2 )
	{
		printf("%s %s %u\r\n", line1, line2,  x );
	}


  getline <"c:\\psxlog.txt" ;
  line1 = $1 ;
  ppc1 = substr($2,7) ;
  getline <"c:\\psxlog2.txt" ;
  line2 = $1 ;
  ppc2 = substr($2,7) ;

if ( ( x % 10000 ) == 0 )
{
   print x ;
}

if ( x > 5360567 )
  break ;
}
}

{ x = 5 ; }
