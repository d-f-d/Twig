<?php
namespace dfd\Twig;

use Drupal\Core\Render\RenderableInterface;
use Twig_Extension;
use Twig_SimpleFilter;
use Twig_SimpleFunction;

class Extension extends Twig_Extension {

  /**
   * @return array
   */
  public function getFilters() {
    return [
      new Twig_SimpleFilter('check_markup', 'check_markup', ['is_safe' => ['html']]),
      new Twig_SimpleFilter('text_summary', 'text_summary', ['is_safe' => ['html']]),
    ];
  }

  /**
   * @return array
   */
  public function getFunctions() {
    return [
      new Twig_SimpleFunction('render_fragment', [$this, 'renderFragment'], ['is_safe' => ['html']]),
      new Twig_SimpleFunction('render_fragment_deferred', [
        $this,
        'renderFragmentDeferredPlaceholder'
      ], ['is_safe' => ['html']]),
    ];
  }

  /**
   * @param string $class
   * @param array $context
   * @return RenderableInterface|string
   */
  public function renderFragment($class) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      return (new \ReflectionClass($class))->newInstanceArgs($args);
    }
    return '';
  }

  /**
   * @param string $class
   * @return array|string
   */
  public function renderFragmentDeferredPlaceholder($class) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      foreach ($args as $i => $arg) {
        if (is_object($arg)) {
          $args[$i] = $arg->id();
        }
      }
      array_unshift($args, $class);
      return [
        '#lazy_builder'       => [get_called_class() . '::renderFragmentDeferred', $args],
        '#create_placeholder' => TRUE,
      ];
    }
    return '';
  }

  static public function renderFragmentDeferred($class) {
    $args = func_get_args();
    array_shift($args);
    $reflection = new \ReflectionClass($class);
    $parameters = $reflection->getMethod('__construct')->getParameters();
    foreach ($args as $i => $arg) {
      if ($parameters[$i]->getClass()) {
        $args[$i] = $parameters[$i]->getClass()->getMethod('load')->invoke(NULL, $arg);
      }
    }
    return $reflection->newInstanceArgs($args)->toRenderable();
  }


  /**
   * @return string
   */
  public function getName() {
    return __CLASS__;
  }
}
