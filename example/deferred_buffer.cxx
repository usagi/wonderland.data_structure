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
    
// note: time_deferred_buffer_t::auto_update is not support for emscripten yet.
#ifndef EMSCRIPTEN
    b -> push_with_update( false );
    b -> auto_update( true );
#endif
    
    constexpr auto interval_in_ms = 100;
    constexpr auto end_of_loops = 15u;
    
    auto counter = 0u;

    const auto step = [&]
    {
      std::cout << "push: " << counter << "\n";
      b -> push( std::to_string( counter ) );
      ++counter;
    };
    
    while( counter < end_of_loops )
    {
      step();
      std::this_thread::sleep_for( std::chrono::milliseconds( interval_in_ms ) );
    }
    
    std::cout << "to dtor\n";
  }
  std::cout << "to exit\n";
}