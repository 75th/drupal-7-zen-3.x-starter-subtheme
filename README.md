# Lanny Heidbreder's starter Zen 7.x-3.x subtheme

This is a Drupal 7 theme that inherits from Zen 7.x-3.x. It has a couple of features from 7.x-5.x grafted in, but for the most part it eschews the ~~massive amounts of pre-written SCSS~~ extra features provided in Zen 5.

This theme is set up to work with the Mac app CodeKit. If you are not using CodeKit, you can delete the config.codekit file and be on your way. If you *are* using CodeKit, it is very important that, when setting up this theme, you follow these steps in this order:

1. Copy the folder to a new folder with the correct name of your new theme.
2. **Immediately** open the theme in CodeKit as a new project. This way, all the subsequent filename changes will be registered by CodeKit.
3. Now batch rename all files called `copy_me.*` to `[yourthemename].*`.
4. Globally find and replace all instances of `copy_me` in all theme files to `[yourthemename]`.
5. Edit the friendly title and description in `[yourthemename].info`.