<?php

  /**
   * @file Theming hook implementations and functionality
   * @author griffinj@lafayette.edu
   *
   */

/**
 * Implements template_preprocess_HOOK().
 */
function dss_yohe_preprocess_islandora_basic_collection_yohe(&$variables) {

  global $base_url;
  global $base_path;
  $islandora_object = $variables['islandora_object'];

  $results = $variables['collection_results'];
  $total_count = count($results);

  $children_pids = array();

  /**
   * Ensure that physical pages containing the "primary material" (e. g. not miscellaneous notes or additional photographs) are ordered first within the sequence of images for the viewer
   * YOHE-7
   *
   */
  $sort = function($u, $v) {
    
    $u_label = $u['title']['value'];
    $v_label = $v['title']['value'];

    if($u_label == $v_label) {

      return 0;
    }

    return preg_match('/\d{3}(r|v)\.tif/', $u_label) ? -1 : 1;

    /*
    if(preg_match('/(r|v)\.tif/', $u_label) or preg_match('/(r|v)\.tif/', $v_label) ) {

      // Yohe-01-001v-ins-002r.tif
      // Yohe-01-001v-ins-002r-Z.tif

      // or

      // Yohe-01-001v-ins-002r-Z.tif
      // Yohe-01-001v-ins-002r.tif

      return preg_match('/(r|v)\.tif/', $u_label) ? -1 : 1;
    } else {

      // Yohe-01-001v-ins-002r-Z.tif
      // Yohe-01-001v-ins-002v-Z.tif

      return preg_match('/(r|v)\.tif/', $u_label) ? -1 : 1;
    }
    */
  };
  usort($results, $sort);

  // Construct the array of Member Objects for the collection...
  for($i=0;$i<$total_count;$i++) {

    $result = $results[$i];

    $children_pids[$result['object']['value']] = array('pid' => $result['object']['value'],
						       'page' => $i + 1);
  }

  /*
   * ...and append this Object for the merged, terminal element:
   * 
   */
  /*
  $children_pids[$islandora_object->id] = array('pid' => $islandora_object->id,
						'page' => count($children_pids));
  */

  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $params = array(
    'object' => $islandora_object,
    'pages' => $children_pids,
    'page_progression' => 'lr',
  );
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  $viewer = islandora_get_viewer($params, 'islandora_book_viewers');
  $variables['viewer'] = $viewer;

  /*
  try {
    $dc = $islandora_object['DC']->content;
    $dc_object = DublinCore::importFromXMLString($dc);
  }
  catch (Exception $e) {
    drupal_set_message(t('Error retrieving object %s %t', array('%s' => $islandora_object->id, '%t' => $e->getMessage())), 'error', FALSE);
  }
  */
  $page_number = (empty($_GET['page'])) ? 0 : $_GET['page'];
  $page_size = (empty($_GET['pagesize'])) ? variable_get('islandora_basic_collection_page_size', '10') : $_GET['pagesize'];
  //$results = $variables['collection_results'];
  //$total_count = count($results);
  //$variables['islandora_dublin_core'] = isset($dc_object) ? $dc_object : array();
  $variables['islandora_object_label'] = $islandora_object->label;
  $display = (empty($_GET['display'])) ? 'list' : $_GET['display'];

  $variables['theme_hook_suggestions'][] = 'islandora_basic_collection_yohe__' . str_replace(':', '_', $islandora_object->id);
}

/**
 * Implements template_preprocess_HOOK().
 */
function dss_yohe_process_islandora_basic_collection_wrapper(&$variables) {

  $page_number = (empty($_GET['page'])) ? 0 : $_GET['page'];
  $page_size = (empty($_GET['pagesize'])) ? variable_get('islandora_basic_collection_page_size', '10') : $_GET['pagesize'];
  $islandora_object = $variables['islandora_object'];
  $results = islandora_basic_collection_get_objects($islandora_object, $page_number, $page_size);

  /**
   * For the rendering of Yohe albums
   *
   */
  $parent_collection = array_shift($islandora_object->getParents());

  if(preg_match('/\:?yohe\:?/', $parent_collection)) {

    // Render the content using the islandora_basic_collection_yohe theming hook
    $collection_content = theme('islandora_basic_collection_yohe', array('islandora_object' => $islandora_object, 'collection_results' => $results));

    unset($variables['discovery_controls']);
  }

  $variables['collection_content'] = $collection_content;
}
