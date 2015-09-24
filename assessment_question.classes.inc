<?php

/**
 * Long answer classes.
 *
 * @file
 *  Classes modelling the assessment question question and the assessment question question response
 */


/**
 * Extension of QuizQuestion.
 */
class AssessmentQuestion extends QuizQuestion {
  /**
   * Implementation of saveNodeProperties
   * @see QuizQuestion#saveNodeProperties($is_new)
   */
  public function saveNodeProperties($is_new = FALSE) {
    if (!isset($this->node->feedback)) {
      $this->node->feedback = '';
    }
    if ($is_new || $this->node->revision == 1) {
      $id = db_insert('quiz_assessment_question_node_properties')
        ->fields(array(
          'nid' => $this->node->nid,
          'vid' => $this->node->vid,
        ))
        ->execute();
    }
    else {
      /*
      db_update('quiz_assessment_question_node_properties')
        ->fields(array('rubric' => $this->node->rubric))
        ->condition('nid', $this->node->nid)
        ->condition('vid', $this->node->vid)
        ->execute();
      */
    }
  }

  /**
   * Implementation of validateNode
   *
   * @see QuizQuestion#validateNode($form)
   */
  public function validateNode(array &$form) { }

  /**
   * Implementation of delete
   *
   * @see QuizQuestion#delete($only_this_version)
   */
  public function delete($only_this_version = FALSE) {
    if ($only_this_version) {
      db_delete('quiz_assessment_question_user_answers')
        ->condition('question_nid', $this->node->nid)
        ->condition('question_vid', $this->node->vid)
        ->execute();
      db_delete('quiz_assessment_question_node_properties')
        ->condition('nid', $this->node->nid)
        ->condition('vid', $this->node->vid)
        ->execute();
    }
    else {
      db_delete('quiz_assessment_question_node_properties')
        ->condition('nid', $this->node->nid)
        ->execute();
      db_delete('quiz_assessment_question_user_answers')
        ->condition('question_nid', $this->node->nid)
        ->execute();
    }
    parent::delete($only_this_version);
  }

  /**
   * Implementation of getNodeProperties
   *
   * @see QuizQuestion#getNodeProperties()
   */
  public function getNodeProperties() {
    if (isset($this->nodeProperties)) {
      return $this->nodeProperties;
    }
    $props = parent::getNodeProperties();

    $res_a = db_query('SELECT rubric FROM {quiz_assessment_question_node_properties}
      WHERE nid = :nid AND vid = :vid', array(':nid' => $this->node->nid, ':vid' => $this->node->vid))->fetchAssoc();

    if (is_array($res_a)) {
      $props = array_merge($props, $res_a);
    }

    $this->nodeProperties = $props;
    return $props;
  }

  /**
   * Implementation of getNodeView
   *
   * @see QuizQuestion#getNodeView()
   */
  public function getNodeView() {
    $content = parent::getNodeView();
    if ($this->viewCanRevealCorrect()) {
      $content['answers'] = array(
        '#type' => 'item',
        '#title' => t('Rubric'),
        '#markup' => '<div class="quiz-solution">' . check_markup($this->node->rubric, $this->getFormat()) . '</div>',
        '#weight' => 1,
      );
    }
    else {
      $content['answers'] = array(
        '#markup' => '<div class="quiz-answer-hidden">Answer hidden</div>',
        '#weight' => 1,
      );
    }
    return $content;
  }

  /**
   * Implementation of getAnweringForm
   *
   * @see QuizQuestion#getAnsweringForm($form_state, $rid)
   */
  public function getAnsweringForm(array $form_state = NULL, $rid) {
    $response = new AssessmentResponse($rid, $this->node);
    return;
/*
    $form = parent::getAnsweringForm($form_state, $rid);
    $form['#theme'] = 'assessment_question_answering_form';

    $form['tries'] = array(
      '#type' => 'textarea',
      '#title' => t('Answer'),
      '#description' => t('Enter your answer here. If you need more space, click on the grey bar at the bottom of this area and drag it down.'),
      '#rows' => 15,
      '#cols' => 60,
      '#required' => FALSE, // See isValid() in AssessmentResponse
    );
    if (isset($rid)) {
      $response = new AssessmentResponse($rid, $this->node);
      $form['tries']['#default_value'] = $response->getResponse();
    }
    return $form;
*/
  }

  /**
   * Implementation of getCreationForm
   *
   * @see QuizQuestion#getCreationForm($form_state)
   */
  public function getCreationForm(array &$form_state = NULL) {
/*
    $form['rubric'] = array(
       '#type' => 'options',
       '#title' => t('Rubric'),
       '#description' => t('Specify the criteria for grading the response.'),
       '#default_value' => isset($this->node->rubric) ? $this->node->rubric : '',
       //'#size' => 60,
       //'#maxlength' => 2048,
       '#key_type' => 'numeric',
       '#key_type_toggle' => t('Customize values'),
       '#key_type_toggled' => FALSE,
       '#required' => FALSE,
    );
    return $form;
*/
  }

  /**
   * Implementation of getMaximumScore
   *
   * @see QuizQuestion#getMaximumScore()
   */
  public function getMaximumScore() {
    return variable_get('assessment_question_default_max_score', 10);
  }

}

/**
 * Extension of QuizQuestionResponse
 */
class AssessmentResponse extends QuizQuestionResponse {

  /**
   * Get all scores that have not yet been evaluated.
   *
   * @param $count
   *  Number of items to return (default: 50).
   * @param $offset
   *  Where in the results we should start (default: 0).
   *
   * @return
   *  Array of objects describing unanswered questions. Each object will have result_id, question_nid, and question_vid.
   */
  public static function fetchAllUnscoredAnswers($count = 50, $offset = 0) {
    global $user;
    $query = db_select('quiz_assessment_question_user_answers', 'a');
    $query->fields('a', array('result_id', 'question_nid', 'question_vid', 'answer_feedback', 'answer_feedback_format'));
    $query->fields('r', array('title'));
    $query->fields('qnr', array('time_end', 'time_start', 'uid'));
    $query->join('node_revision', 'r', 'a.question_vid = r.vid');
    $query->join('quiz_node_results', 'qnr', 'a.result_id = qnr.result_id');
    $query->join('node', 'n', 'qnr.nid = n.nid');
    $query->condition('a.is_evaluated', 0);
    if (user_access('score own quiz') && user_access('score taken quiz answer')) {
      $query->condition(db_or()->condition('n.uid', $user->uid)->condition('qnr.uid', $user->uid));
    }
    else if (user_access('score own quiz')) {
      $query->condition('n.uid', $user->uid);
    }
    else if (user_access('score taken quiz answer')) {
      $query->condition('qnr.uid', $user->uid);
    }
    $results = $query->execute();
    $unscored = array();
    foreach ($results as $row) {
      $unscored[] = $row;
    }
    return $unscored;
  }

  /**
   * Given a quiz, return a list of all of the unscored answers.
   *
   * @param $nid
   *  Node ID for the quiz to check.
   * @param $vid
   *  Version ID for the quiz to check.
   * @param $count
   *  Number of items to return (default: 50).
   * @param $offset
   *  Where in the results we should start (default: 0).
   *
   * @return
   *  Indexed array of result IDs that need to be scored.
   */
  public static function fetchUnscoredAnswersByQuestion($nid, $vid, $count = 50, $offset = 0) {
    $results = db_query('SELECT result_id FROM {quiz_assessment_question_user_answers}
      WHERE is_evaluated = :is_evaluated
      AND question_nid = :question_nid
      AND question_vid = :question_vid', array(':is_evaluated' => 0, ':question_nid' => $nid, ':question_vid' => $vid));
    $unscored = array();
    foreach ($results as $row) {
      $unscored[] = $row->result_id;
    }
    return $unscored;
  }

  /**
   * ID of the answer.
   */
  protected $answer_id = 0;

  /**
   * Constructor
   */
  public function __construct($result_id, stdClass $question_node, $answer = NULL) {
    parent::__construct($result_id, $question_node, $answer);

    if (!isset($answer)) {
      // Question has been answered allready. We fetch the answer data from the database.
      $r = db_query('SELECT answer_id, answer, is_evaluated, score, question_vid, question_nid, result_id, answer_feedback, answer_feedback_format
        FROM {quiz_assessment_question_user_answers}
        WHERE question_nid = :qnid AND question_vid = :qvid AND result_id = :rid', array(':qnid' => $question_node->nid, ':qvid' => $question_node->vid, ':rid' => $result_id))->fetch();

      if (!empty($r)) {
        $this->answer = $r->answer;
        $this->score = $r->score;
        $this->evaluated = $r->is_evaluated;
        $this->answer_id = $r->answer_id;
        $this->answer_feedback = $r->answer_feedback;
        $this->answer_feedback_format = $r->answer_feedback_format;
      }
    }
    else {
      $this->answer = $answer;
      $this->evaluated = FALSE;
    }
  }

  /**
   * Implementation of isValid
   *
   * @see QuizQuestionResponse#isValid()
   */
  public function isValid() {
    return TRUE;
  }

  /**
   * Implementation of save
   *
   * @see QuizQuestionResponse#save()
   */
  public function save() {
    $this->answer_id = db_insert('quiz_assessment_question_user_answers')
      ->fields(array(
        'answer' => 0, 
        'question_nid' => $this->question->nid,
        'question_vid' => $this->question->vid,
        'result_id' => $this->rid,
      ))
      ->execute();
  }

  /**
   * Implementation of delete
   *
   * @see QuizQuestionResponse#delete()
   */
  public function delete() {
    db_delete('quiz_assessment_question_user_answers')
      ->condition('question_nid', $this->question->nid)
      ->condition('question_vid', $this->question->vid)
      ->condition('result_id', $this->rid)
      ->execute();
  }

  /**
   * Implementation of score
   *
   * @see QuizQuestionResponse#score()
   */
  public function score() {
    return (int) db_query('SELECT score FROM {quiz_assessment_question_user_answers}
      WHERE result_id = :result_id AND question_vid = :question_vid', array(':result_id' => $this->rid, ':question_vid' => $this->question->vid))->fetchField();
  }

  /**
   * Implementation of getResponse
   *
   * @see QuizQuestionResponse#getResponse()
   */
  public function getResponse() {
    return $this->answer;
  }

  /**
   * Implementation of getReportFormResponse
   *
   * @see QuizQuestionResponse#getReportFormResponse($showpoints, $showfeedback, $allow_scoring)
   */
  public function getReportFormResponse($showpoints = TRUE, $showfeedback = TRUE, $allow_scoring = FALSE) {
    $form = array();
    $form['#theme'] = 'assessment_question_response_form';
    if ($this->question && !empty($this->question->answers)) {
      $answer = (object) current($this->question->answers);
    }
    else {
      return $form;
    }
    // $form['answer'] = array('#markup' => check_markup($answer->answer));
    if ($answer->is_evaluated == 1) {
      // Show feedback, if any.
      if ($showfeedback && !empty($answer->feedback)) {
        $feedback = check_markup($answer->feedback);
      }
    }
    else {
      $feedback = t('This answer has not yet been scored.') .
        '<br/>' .
        t('Until the answer is scored, the total score will not be correct.');
    }
    if ($allow_scoring) {
      $question = node_load($this->question->nid);
      $rubric_options_result = field_get_items('node', $question, 'field_rubric_options');
      $options = array();
      foreach ($rubric_options_result as $key => $value) {
        $options[$key] = $value['safe_value']; 
      }
      $question->content = node_view($question, 'full');
      $form['rubric'] = array(
        '#type' => 'radios',
        '#title' => $question->title,
        '#description' => $question->content['body'][0]['#markup'],
        '#options' => $options,
        '#default_value' => $this->answer,
        '#theme' => 'assessment_question_rubric_radios',
        '#attributes' => array('class' => array('rubric')),
      ); 
    }
    if (!$allow_scoring && !empty($this->answer_feedback)) {
      $form['answer_feedback'] = array(
        '#title' => t('Feedback'),
        '#type' => 'item',
        '#markup' => '<span class="quiz_answer_feedback">' . check_markup($this->answer_feedback, $this->answer_feedback_format) . '</span>',
      );
    }
    if (!empty($feedback)) {
      $form['feedback'] = array('#markup' => '<span class="quiz_answer_feedback">' . $feedback . '</span>');
    }
    $form['#attached']['css'][] = drupal_get_path('module', 'assessment_question') . '/theme/assessment-question-radios.css';
    $form['#attached']['js'][] = drupal_get_path('module', 'assessment_question') . '/theme/assessment-question-radios.js';
    return $form;
  }

  /**
   * Implementation of getReportFormScore
   *
   * @see QuizQuestionResponse#getReportFormScore($showpoints, $showfeedback, $allow_scoring)
   */
  public function getReportFormScore($showfeedback = TRUE, $showpoints = TRUE, $allow_scoring = FALSE) {
    // The score will be shown as a questionmark if the question haven't been scored already
    $score = ($this->isEvaluated()) ? $this->getScore() : 0;
    // We show a textfield if the quiz shall be scored. Markup otherwise
    if (quiz_access_to_score() && $allow_scoring) {
      return array(
        '#type' => 'textfield',
        '#default_value' => $score,
        '#size' => 3,
        '#maxlength' => 3,
        '#attributes' => array('class' => array('quiz-report-score')),
      );
    }
    else {
      return array('#markup' => $score);
    }
  }

  public function getReportFormAnswerFeedback($showpoints = TRUE, $showfeedback = TRUE, $allow_scoring = FALSE) {
    if (quiz_access_to_score() && $allow_scoring) {
      return array(
        '#title' => t('Feedback'),
        '#type' => 'textarea',
        '#rows' => 2,
        '#default_value' => isset($this->answer_feedback) ? check_markup($this->answer_feedback, $this->answer_feedback_format) : '',
        '#format' => isset($this->answer_feedback_format) ? $this->answer_feedback_format : NULL,
        '#attributes' => array('class' => array('quiz-report-score')),
      );
    }
    return FALSE;
  }


  /**
   * Implementation of getReportFormSubmit
   *
   * @see QuizQuestionResponse#getReportFormSubmit($showfeedback, $showpoints, $allow_scoring)
   */
  public function getReportFormSubmit($showfeedback = TRUE, $showpoints = TRUE, $allow_scoring = FALSE) {
    return $allow_scoring ? 'assessment_question_report_submit' : FALSE;
  }

  /**
   * Implementation of getReportFormValidate
   *
   * @see QuizQuestionResponse#getReportFormValidate($showfeedback, $showpoints, $allow_scoring)
   */
  public function getReportFormValidate($showfeedback = TRUE, $showpoints = TRUE, $allow_scoring = FALSE) {
    return $allow_scoring ? 'assessment_question_report_validate' : FALSE;
  }

  /**
   * Check to see if the answer is marked as correct.
   *
   * This default version returns TRUE iff the score is equal to the maximum possible score.
   */
  function getScore($weight_adjusted = TRUE) {
    if ($this->is_skipped) {
      return 0;
    }
    if (!isset($this->score)) {
      $this->score = $this->score();
    }
    if (isset($this->question->score_weight) && $weight_adjusted) {
      return round($this->score * $this->question->score_weight);
    }
    return $this->score;
  }
}

