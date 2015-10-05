<?php
$p = drupal_get_path('module', 'quiz') .'/theme/';
$q_image = $p. 'question_bg.png';
$c_class = ($form['#is_evaluated']) ? ($form['#is_correct']) ? 'q-correct' : 'q-wrong' : 'q-waiting';
?>

<div class="dt">
  <div class="quiz-report-score-container <?php print $c_class?>">
  	<span>
      <?php print t('Score')?>
      <?php print drupal_render($form['score'])?>
      <?php print t('of') .' '. $form['max_score']['#value']?>
      <?php if ($form['#is_skipped']): ?>
        <br><em><span class="quiz-report-skipped">
        <?php print t('(skipped)') ?>
        </span></em>
      <?php endif; ?>
      <?php if ($form['#is_doubtful']): ?>
        <br><em><span class="quiz-report-doubtful">
        <?php print t('(Doubtful)') ?>
        </span></em>
      <?php endif; ?>
    </span>
  </div>
  <?php //print drupal_render($form['question']);?>
</div>
<?php if (!isset($form['response']['#no-response'])): ?>
<div class="dd">
  <?php  print drupal_render($form['response']); ?>
</div>
<?php endif; ?>
<?php // print drupal_render($form['answer_feedback']); ?>
