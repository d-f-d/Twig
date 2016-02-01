<?php
namespace d\Twig\BEM;

use d\Twig\Compiler;
use Twig_Compiler;
use Twig_Node;
use Twig_NodeInterface;

class Node extends Twig_Node {
  /**
   * @var Compiler
   */
  private $compiler;
  static private $blocks = [];

  /**
   * @param Twig_Compiler $compiler
   * @return $this
   */
  private function start(Twig_Compiler $compiler) {
    $this->compiler = $compiler;
    $this->compiler->addDebugInfo($this);
    return $this;
  }

  /**
   * @param Twig_Compiler $compiler
   */
  public function compile(Twig_Compiler $compiler) {
    $this->start($compiler)->write('');
    if ($this->getAttribute('method') !== 'setBlock') {
      $this
        ->raw('echo "class=\'"')
        ->{$this->getAttribute('method')}($compiler)
        ->appendPrimitive("'");
    }
    else {
      $this->setBlock($compiler);
    }
    $this->raw(";\n");
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
   * @param string $s
   * @return $this
   */
  private function raw($s) {
    $this->compiler->raw($s);
    return $this;
  }

  /**
   * @param string $s
   * @return $this
   */
  private function write($s) {
    $this->compiler->write($s);
    return $this;
  }

  /**
   * @param Twig_NodeInterface $node
   * @return $this
   */
  private function subcompile(Twig_NodeInterface $node) {
    $this->compiler->subcompile($node);
    return $this;
  }

  /**
   * @param mixed|Twig_NodeInterface $v
   * @return $this
   */
  private function append($v) {
    if (is_object($v) && 'Twig_Node_Expression_Constant' === get_class($v)) {
      return $this->appendPrimitive($v->attributes['value']);
    }
    return $this->raw(' . ')->subcompile($v);
  }

  /**
   * @param mixed $v
   * @return $this
   */
  private function appendPrimitive($v) {
    $source = $this->compiler->getSource();
    if ('"' === $source[$pos = (strlen($source) - 1)]) {
      $source = substr_replace($source, $v, $pos, 0);
      $this->compiler->setSource($source);
    }
    else {
      $this->raw(' . "' . $v . '"');
    }
    return $this;
  }

  /**
   * @return $this
   */
  private function echoMore() {
    if (('Twig_Node_Expression_Constant' === get_class($more = $this->getNode('more')))) {
      $this->appendPrimitive(' ')->append($more);
    }
    else {
      $this
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
    $class = get_class($mod);
    if ('Twig_Node_Expression_Constant' === $class && empty($mod->attributes['value'])) {
      return $this;
    }
    if ('Twig_Node_Expression_Array' === $class) {
      for ($i = 1; $i < count($mod->nodes); $i += 2) {
        if ('Twig_Node_Expression_Constant' === get_class($mod->nodes[$i])) {
          $this
            ->appendPrimitive(' ')
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
    if ('Twig_Node_Expression_Constant' === $class) {
      return $this
        ->appendPrimitive(' ')
        ->element()
        ->appendPrimitive('--')
        ->append($mod);
    }
    return $this
      ->raw(' . (($mod_a = array_filter(is_array($mod = ')
      ->subcompile($mod)
      ->raw(') ? $mod : [$mod])) ? (($be = (" "')
      ->element()->appendPrimitive('--')
      ->raw(')) . implode($be, $mod_a)) : "")');
  }

  private function setBlock(Twig_Compiler $compiler) {
    if ('Twig_Node_Expression_Constant' === get_class($b = $this->getNode('block'))) {
      static::$blocks[$compiler->getFilename()] = $b->attributes['value'];
    }
    else {
      unset(static::$blocks[$compiler->getFilename()]);
    }
    $this
      ->raw('$context[\'block\'] = ')
      ->subcompile($b);
  }

  /**
   * @return $this
   */
  private function block() {
    return $this
      ->echoBlock();
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