window.onload = function(){
  canvSize = 600;
  ranks = files = 8;
  squareSize = canvSize / ranks;
  mouseCol = -1;
  mouseRow = -1;
  selSquare = -1;

  pieces = [];
  fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  fen = "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 w k - 0 13";

  configureCanvas();
  setInterval(drawCanvas, 1000/5);

}

function configureCanvas(){
  let canvas = document.getElementById("canv");
  canvas.width = canvSize;
  canvas.height = canvSize;
  //canvas.addEventListener("mousemove", (event) => {trackmouse(event);});
  canvas.addEventListener("mousedown", (event) => {trackmouse(event);});
}

function drawCanvas(){
  let canv = document.getElementById("canv");
  let ctx = canv.getContext("2d");
  let light = {
    r: 255,
    g: 224,
    b: 102
  };
  let dark = {
    r: 128,
    g: 102,
    b: 0
  };

  for(let y = 0 ; y < files ; y++){
    for(let x = 0 ; x < ranks ; x++){
      let color = {};
      Object.assign(color, (((x + y) % 2 == 0) ? light : dark));
      if(rc2Index(y,x) == selSquare){
        color.r *= 1.8 ; color.g *= 1.8 ; color.b *= 1.8 ;
      }

      ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
      ctx.fillRect(x * squareSize, y * squareSize, squareSize, squareSize);

      // Adding lettering and numbering on the sides
      if(x == 0){
        ctx.textAlign = "left";
        ctx.textBaseline = "top";
        color = (((x + y) % 2 != 0) ? light : dark);
        ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
        ctx.font = "24px Arial";
        ctx.fillText(8-y, x * squareSize, y * squareSize);
      }
      if(y == 7){
        ctx.textAlign = "left";
        ctx.textBaseline = "bottom";
        color = (((x + y) % 2 != 0) ? light : dark);
        ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
        ctx.font = "24px Arial";
        ctx.fillText(String.fromCharCode(x+97), x * squareSize, (y+1) * squareSize);
      }
    }
  }
  drawPieces();
}

function trackmouse(event){
  let canvas = document.getElementById("canv");
  let offset = cumulativeOffset(canvas);
  let mouseX = event.clientX - offset.left;
  let mouseY = event.clientY - offset.top;

  let col = Math.floor(mouseX / squareSize);
  let row = Math.floor(mouseY / squareSize);

  mouseCol = col;
  mouseRow = row;

  selectSquare();

}

var selectSquare = function(){
  console.log(rc2Index(mouseRow, mouseCol));
  if(selSquare == -1){
    selSquare = rc2Index(mouseRow, mouseCol);
  } else {
    let piece = pieces[selSquare];
    pieces[selSquare] = "-";
    pieces[rc2Index(mouseRow,mouseCol)] = piece;
    selSquare = -1;
  }
}

var cumulativeOffset = function(element) {
  var top = 0, left = 0;
  do {
    top += element.offsetTop || 0;
    left += element.offsetLeft || 0;
    element = element.offsetParent;
  } while(element);

  return {
    top: top,
    left: left
  };
};

var square = function(row, col){
  return String.fromCharCode(col+97) + (8-row);
}

var rc2Index = function(row, col){
  return 8 * row + col;
}

var index2RC = function(row, col){
  let x = i % 8;
  let y = (i-x) / 8;
  return (x,y);
}

var setFEN = function(fen){
  let piece;
  let rank = 8;
  let file = 1;
  for(let i = 0 ; i < fen.length && rank > 0 ; i++){
    let count = 1;
    switch(fen[i]){
      case "p":
      case "r":
      case "n":
      case "b":
      case "q":
      case "k":
      case "P":
      case "R":
      case "N":
      case "B":
      case "Q":
      case "K":
        piece = fen[i];
        break;
      case "1":
      case "2":
      case "3":
      case "4":
      case "5":
      case "6":
      case "7":
      case "8":
        piece = "-"
        count = parseInt(fen[i]);
        break;
      case "/":
      case " ":
        rank--;
        file = 1;
        continue;
      default:
        console.log("Error with FEN!");
    }
    for(let x = 0 ; x < count ; x++){
      let boardIndex = 8 * (8-rank) + file - 1;
      pieces[boardIndex] = piece;
      file++;
    }
  }
}

// This can be written better
var genFEN = function(){
  let fen = '';
  let empty = 0;
  for(let i = 0 ; i < pieces.length ; i++){
    if(i % 8 == 0 && i > 0){
      if(empty){ fen += empty; empty = 0; }
      fen += "/";
    }
    if(pieces[i] == "-"){
      empty++;
    } else {
      if(empty){ fen += empty; empty = 0; }
      fen += pieces[i];
    }
  }
  if(empty){ fen += empty; empty = 0; }
  return fen;
}

var drawPieces = function(){
  let canv = document.getElementById("canv");
  let ctx = canv.getContext("2d");
  let path;
  for(let i = 0 ; i < pieces.length ; i++){
    switch(pieces[i]){
      case "p": path = "./pieces/pawn_black_60.png"; break;
      case "r": path = "./pieces/rook_black_60.png"; break;
      case "n": path = "./pieces/night_black_60.png"; break;
      case "b": path = "./pieces/bishop_black_60.png"; break;
      case "q": path = "./pieces/queen_black_60.png"; break;
      case "k": path = "./pieces/king_black_60.png"; break;
      case "P": path = "./pieces/pawn_white_60.png"; break;
      case "R": path = "./pieces/rook_white_60.png"; break;
      case "N": path = "./pieces/night_white_60.png"; break;
      case "B": path = "./pieces/bishop_white_60.png"; break;
      case "Q": path = "./pieces/queen_white_60.png"; break;
      case "K": path = "./pieces/king_white_60.png"; break;
      default: path = null;
    }
    if(path){
      let imgObj = new Image();
      imgObj.src = path;
      let x = i % 8;
      let y = (i-x) / 8;
      ctx.drawImage(
        imgObj,
        (x + 0.5) * squareSize - imgObj.width / 2,
        (y + 0.5) * squareSize - imgObj.height / 2
      );
    }
  }
}

function formatPOSTData(object) {
  var encodedString = '';
  for (var key in object) {
    if (object.hasOwnProperty(key)) {
      if (encodedString.length > 0) {
        encodedString += '&';
      }
      encodedString += encodeURI(key + '=' + object[key]);
    }
  }
  return encodedString;
}

var saveFEN = function(){
  let fen = genFEN();
  let url = './handle_input.php';

  let xhr = new XMLHttpRequest();
  xhr.open('POST', url);
  // xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhr.onload = function(){
    if (xhr.status !== 200){
      alert('Something went wrong with submission.  Please contact Brendan.')
    }
    console.log(xhr.status);
  };
  let payload = {
    fen: fen
  };
  xhr.send(formatPOSTData(payload));
}