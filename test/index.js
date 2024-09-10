const mouseListener = require('../build/Release/io_listener');

// Start listening for mouse and keyboard events
mouseListener.startListening((event) => {
  console.log('Event received:', event);

  if (event.type === 'mousemove') {
    console.log(`Mouse moved to (${event.x}, ${event.y})`);
  } else if (event.type === 'mousedown') {
    console.log(`Mouse down at (${event.x}, ${event.y})`);
  } else if (event.type === 'mouseup') {
    console.log(`Mouse up at (${event.x}, ${event.y})`);
  } else if (event.type === 'scroll') {
    console.log(`Mouse wheel scrolled by ${event.delta}`);
  } else if (event.type === 'keydown') {
    console.log(`Key down: ${event.keycode}`);
  }
});
