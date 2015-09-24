/**
 * @file
 */

(function ($, Drupal, window, document, undefined) {

    Drupal.behaviors.Rubric = {
        attach: function(context, settings) {
          $('.form-type-radios .rubric .form-type-radio input').hide();
          $('input.rubric:radio:checked').parent('div').addClass('pre-clicked');
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
