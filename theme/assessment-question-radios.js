/**
 * @file
 * A JavaScript file for the theme.
 *
 * In order for this JavaScript to be loaded on pages, see the instructions in
 * the README.txt next to this file.
 */

// JavaScript should be made compatible with libraries other than jQuery by
// wrapping it with an "anonymous closure". See:
// - https://drupal.org/node/1446420
// - http://www.adequatelygood.com/2010/3/JavaScript-Module-Pattern-In-Depth
(function ($, Drupal, window, document, undefined) {


// To understand behaviors, see https://drupal.org/node/756722#behaviors
    Drupal.behaviors.Rubric = {
        attach: function(context, settings) {
          $('.form-type-radios .rubric .form-type-radio input').hide();
          $('input.rubric:radio:checked').parent('div').addClass('clicked');
          if($('.form-type-radios .rubric .form-type-radio').find('input.rubric').is(':checked')) {
              $(this).addClass('clicked');
          }
          $('.form-type-radios .rubric .form-type-radio').click(function() {
              $(this).find('input.rubric').attr( "checked", "checked");
              $('.form-type-radios .rubric .form-type-radio').removeClass('clicked');
              $(this).addClass('clicked');
          });

        }
    };


})(jQuery, Drupal, this, this.document);
