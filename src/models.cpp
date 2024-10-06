#include "models.hpp"
#include <iostream>


namespace halloween {
    typedef std::vector<std::string_view> ModelView;

    const std::string STR_PLAYER = "\
 __\n\
 ||\n\
<TT)==\n\
 /\\";

    const std::string STR_BULLET = "=>";

    const std::string STR_PUMPKIN = "\
 _//_\n\
/.  .\\\n\
\\_WW_/";

    const std::string STR_ZOMBIE = "\
  /--\\\n\
  \\--/\n\
==(##|\n\
   ||";

    Model MODEL_PLAYER;
    Model MODEL_BULLET;
    Model MODEL_PUMPKIN;
    Model MODEL_ZOMBIE;

    void init_texture(Model & m, const ModelView & strs) {
        for (unsigned int i = 0; i < strs.size(); i++) {
            m.emplace_back();
            for (const auto & c : strs[i]) {
                m[i].emplace_back(c | A_NORMAL | COLOR_PAIR(0));
            }
        }
    }

    void initialize_textures() {
        init_texture(MODEL_PLAYER, ModelView{{STR_PLAYER}});
        init_texture(MODEL_BULLET, ModelView{{STR_BULLET}});
        init_texture(MODEL_PUMPKIN, ModelView{{STR_PUMPKIN}});
        init_texture(MODEL_ZOMBIE, ModelView{{STR_ZOMBIE}});
    }

}
