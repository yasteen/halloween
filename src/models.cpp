#include "models.hpp"
#include <iostream>


namespace halloween {
    struct TextureView {
        std::string_view tex;
        unsigned int color;

        TextureView(std::string_view tex): tex{tex}, color{0} {};
        TextureView(std::string_view tex, unsigned int color): tex{tex}, color{color} {};
    };
    typedef std::vector<TextureView> ModelView;

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

    const std::string STR_BAT = "\
,,_,.-_\n\
/)\\=|` \\\n\
 \\/.-.-.\\\n\
 \"      `";

    Model MODEL_PLAYER;
    Model MODEL_BULLET;
    Model MODEL_PUMPKIN;
    Model MODEL_ZOMBIE;
    Model MODEL_BAT;

    void init_texture(Model & m, const ModelView & textures) {
        for (unsigned int i = 0; i < textures.size(); i++) {
            m.emplace_back(std::vector<chtype>{});
            for (const auto & c : textures[i].tex) {
                m[i].tex.emplace_back(c | A_NORMAL | COLOR_PAIR(0));
            }
            m[i].color = textures[i].color;
        }
    }

    void initialize_textures() {
        // Leave 0 for default
        if (has_colors()) {
            start_color();
            use_default_colors();
            init_pair(COLOR_HURT, COLOR_RED, -1);
            init_pair(COLOR_SELECT, -1, COLOR_BLUE);
        }
        init_texture(MODEL_PLAYER, ModelView{{STR_PLAYER, 0}, {STR_PLAYER, 1}});
        init_texture(MODEL_BULLET, ModelView{{STR_BULLET}});
        init_texture(MODEL_PUMPKIN, ModelView{{STR_PUMPKIN}});
        init_texture(MODEL_ZOMBIE, ModelView{{STR_ZOMBIE}});
        init_texture(MODEL_BAT, ModelView{{STR_BAT}});
    }

}
