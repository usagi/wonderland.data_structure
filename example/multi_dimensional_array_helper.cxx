#include <iostream>

#include <wonder_rabbit_project/wonderland/data_structure.hxx>

auto main()
  -> int
{
  using wonder_rabbit_project::wonderland::data_structure::multi_dimensional_array_helper;
  
  multi_dimensional_array_helper< std::uint8_t, 3, 5, 7 >::type mda;
  
  {
    std::uint8_t n = 0;
    
    for ( auto& d1 : mda )
      for ( auto& d2 : d1 )
        for ( auto& value : d2 )
          value = n++;
  }
  
  std::cout << + mda[2][4][6] << std::endl;
  
  try
  { mda.at(5).at(0).at(0) = 210; }
  catch ( const std::exception& e )
  { std::cerr << "exception( it is succeeded of the exception testing ): " << e.what() << "\n"; }
  
}