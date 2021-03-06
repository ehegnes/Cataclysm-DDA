#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#include "game.h"
#include "filesystem.h"
#include "init.h"
#include "map.h"
#include "morale.h"
#include "path_info.h"
#include "player.h"
#include "worldfactory.h"
#include "debug.h"

void init_global_game_state() {
    PATH_INFO::init_base_path("");
    PATH_INFO::init_user_dir("./");
    PATH_INFO::set_standard_filenames();

    if( !assure_dir_exist( FILENAMES["config_dir"] ) ) {
        assert( !"Unable to make config directory. Check permissions." );
    }

    if( !assure_dir_exist( FILENAMES["savedir"] ) ) {
        assert( !"Unable to make save directory. Check permissions." );
    }

    if( !assure_dir_exist( FILENAMES["templatedir"] ) ) {
        assert( !"Unable to make templates directory. Check permissions." );
    }

    get_options().init();
    get_options().load();
    init_colors();

    g = new game;

    g->load_static_data();
    g->load_core_data();
    DynamicDataLoader::get_instance().finalize_loaded_data();

    world_generator->set_active_world(NULL);
    world_generator->get_all_worlds();
    WORLDPTR test_world = world_generator->make_new_world( false );
    assert( test_world != NULL );
    world_generator->set_active_world(test_world);
    assert( world_generator->active_world != NULL );

    g->u = player();
    g->u.create(PLTYPE_NOW);
    g->m = map( static_cast<bool>( ACTIVE_WORLD_OPTIONS["ZLEVELS"] ) );

    g->m.load( g->get_levx(), g->get_levy(), g->get_levz(), false );
}

int main( int argc, const char *argv[] )
{
  debug_fatal = true; // prevents stalling on debugmsg, see issue #15723

  // TODO: Only init game if we're running tests that need it.
  init_global_game_state();

  int result = Catch::Session().run( argc, argv );

  auto world_name = world_generator->active_world->world_name;
  if (result == 0) {
      g->delete_world(world_name, true);
  } else {
      printf("Test world \"%s\" left for inspection.\n", world_name.c_str());
  }

  return result;
}
