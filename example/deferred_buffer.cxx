#include <iostream>

#include <wonder_rabbit_project/wonderland/data_structure.hxx>

auto main()
  -> int
{
  {
    using tdb_t = wonder_rabbit_project::wonderland::data_structure::time_deferred_buffer_t<>;
    auto b = std::make_shared< tdb_t >();
    
    b -> flush
    ( []( const tdb_t::flush_parameter_t& b, const tdb_t::flush_parameter_t& e )
      {
        for ( auto i = b; i != e; ++i )
          std::cout << "flush: " << i -> second << "\n";
      }
    );
    
    b -> delay( std::chrono::milliseconds( 500 ) );
    b -> push_with_update( false );
    b -> auto_update( true );
    
    auto counter = 0ull;
    
    while( counter < 20 )
    {
      std::cout << "push: " << counter << "\n";
      b -> push( std::to_string( counter ) );
      ++counter;
      std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    }
    
    std::cout << "to dtor\n";
  }
  std::cout << "to exit\n";
}