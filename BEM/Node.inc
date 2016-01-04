<?php
namespace d\Twig\BEM;

use d\Twig\Compiler;
use Twig_Compiler;
use Twig_Node;

class Node extends Twig_Node {
  /**
   * @var Compiler
   */
  private $compiler;
  static private $blocks = [];

  public function __construct(array $nodes = [], array $attributes = [], $line, $tag = NULL) {
    parent::__construct($nodes, $attributes, $line, $tag);
  }

  /**
   * @param \Twig_Compiler $compiler
   * @return $this
   */
  private function start(Twig_Compiler $compiler) {
    $this->compiler = $compiler;
    $this->compiler
      ->addDebugInfo($this);
    return $this;
  }

  public function compile(Twig_Compiler $compiler) {
    $this->start($compiler);
    if ($this->getAttribute('method') !== 'setBlock') {
      $this->compiler->write('echo "class=\'"');
      $this->{$this->getAttribute('method')}($compiler);
      $this->appendPrimitive("'");
      $this->compiler->raw(";\n");
    }
    else {
      $this->{$this->getAttribute('method')}($compiler);
    }
  }

  /**
   * @return $this
   */
  private function echoBlock() {
    if (isset(static::$blocks[$this->compiler->getFilename()])) {
      return $this->appendPrimitive(static::$blocks[$this->compiler->getFilename()]);
    }
    return $this->appendPrimitive('{$context[\'block\']}');
  }

  /**
   * @param $v
   * @return $this
   */
  private function append($v) {
    if (is_object($v) && get_class($v) === 'Twig_Node_Expression_Constant') {
      return $this->appendPrimitive($v->attributes['value']);
    }
    $this->compiler
      ->raw(' . ')->subcompile($v);
    return $this;
  }

  /**
   * @param $v
   * @return $this
   */
  private function appendPrimitive($v) {
    $source = $this->compiler->getSource();
    if ($source[$pos = (strlen($source) - 1)] === '"') {
      $source = substr_replace($source, $v, $pos, 0);
      $this->compiler->setSource($source);
    }
    else {
      $this->compiler->raw(' . "' . $v . '"');
    }
    return $this;
  }

  /**
   * @return $this
   */
  private function echoMore() {
    if ((get_class($more = $this->getNode('more')) === 'Twig_Node_Expression_Constant')) {
      $this->appendPrimitive(' ')->append($more);
    }
    else {
      $this->compiler
        ->raw(' . ')
        ->raw('(($v = ')
        ->subcompile($more)
        ->raw(') ? (" " . $v) : "")');
    }
    return $this;
  }

  /**
   * @return $this
   */
  private function echoElement() {
    return $this
      ->appendPrimitive('__')
      ->append($this->getNode('element'));
  }

  /**
   * @return $this
   */
  private function echoMod() {
    $mod = $this->getNode('mod');
    $c = get_class($mod);
    if ($c === 'Twig_Node_Expression_Constant' && empty($mod->attributes['value'])) {
      return $this;
    }
    if ($c === 'Twig_Node_Expression_Array') {
      for ($i = 1; $i < count($mod->nodes); $i += 2) {
        if (get_class($mod->nodes[$i]) === 'Twig_Node_Expression_Constant') {
          $this->appendPrimitive(' ')
            ->element()
            ->appendPrimitive('--')
            ->append($mod->nodes[$i]);
          $mod->nodes[$i] = $mod->nodes[$i - 1] = NULL;
        }
      }
      $mod->nodes = array_filter($mod->nodes);
      if (!$mod->nodes) {
        return $this;
      }
    }
    if ($c === 'Twig_Node_Expression_Constant') {
      return $this
        ->appendPrimitive(' ')
        ->element()
        ->appendPrimitive('--')
        ->append($mod);
    }
    $this->compiler->raw(' . (($mod_a = array_filter(is_array($mod = ');
    $this->compiler->subcompile($mod);
    $this->compiler->raw(') ? $mod : [$mod])) ? (($be = (" "');
    $this->element()->appendPrimitive('--');
    $this->compiler->raw(')) . implode($be, $mod_a)) : "")');
    return $this;
  }

  private function setBlock(Twig_Compiler $compiler) {
    if (get_class($b = $this->getNode('block')) === 'Twig_Node_Expression_Constant') {
      static::$blocks[$compiler->getFilename()] = $b->attributes['value'];
    }
    else {
      unset(static::$blocks[$compiler->getFilename()]);
    }
    $compiler
      ->write('$context[\'block\'] = ')
      ->subcompile($b)
      ->raw(";\n");
  }

  /**
   * @return $this
   */
  private function block() {
    return $this->echoBlock();
  }

  /**
   * @return $this
   */
  private function blockMore() {
    return $this
      ->echoBlock()
      ->echoMore();
  }

  /**
   * @return $this
   */
  private function element() {
    return $this
      ->echoBlock()
      ->echoElement();
  }

  /**
   * @return $this
   */
  private function elementMod() {
    return $this
      ->element()
      ->echoMod();
  }

  /**
   * @return $this
   */
  private function elementModMore() {
    return $this
      ->elementMod()
      ->echoMore();
  }

}