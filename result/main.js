import { parse } from 'csv-parse';
import { stringify } from 'csv-stringify';
import fs from 'fs';
function main() {
  parse(fs.readFileSync('./performance_unknown_linux64_gcc7.5.csv'), {
    columns: false,
    skipEmptyLines: true
  }, function (err, records) {
    let lastName = '';
    let lastRecord;
    const results = [];
    for (let record of records) {
      if (lastName !== record[1]) {
        lastName = record[1];
        lastRecord = {
          name: lastName,
          parseTime: 0,
          parseMem: 0,
          parseMemPeak: 0,
          stringifyTime: 0,
          stringifyMem: 0,
          stringifyMemPeak: 0,
          prettifyTime: 0,
          prettifyMem: 0,
          prettifyMemPeak: 0,
          fileSize: 0
        };
      }
      if (record[0] === '1. Parse') {
        lastRecord.parseTime += Number(record[3]);
        lastRecord.parseMem += Number(record[4]);
        lastRecord.parseMemPeak = Math.max(Number(record[5]), lastRecord.parseMemPeak);
      } else if (record[0] === '2. Stringify') {
        lastRecord.stringifyTime += Number(record[3]);
        lastRecord.stringifyMem += Number(record[4]);
        lastRecord.stringifyMemPeak = Math.max(Number(record[5]), lastRecord.stringifyMemPeak);
      } else if (record[0] === '3. Prettify') {
        lastRecord.prettifyTime += Number(record[3]);
        lastRecord.prettifyMem += Number(record[4]);
        lastRecord.prettifyMemPeak = Math.max(Number(record[5]), lastRecord.prettifyMemPeak);
      } else if (record[0] === '7. Code size') {
        lastRecord.fileSize = Number(record[7]);
        results.push(lastRecord);
      }
    }
    const matrix = Array.from(Array(13), item => new Array(results.length).fill(''));
    matrix[1][0] = 'Mem';
    matrix[2][0] = 'Mem Peak';
    matrix[3][0] = 'ROM';
    matrix[4][0] = 'parseTime';
    matrix[5][0] = 'parseMem';
    matrix[6][0] = 'parseMemPeak';
    matrix[7][0] = 'stringifyTime';
    matrix[8][0] = 'stringifyMem';
    matrix[9][0] = 'stringifyMemPeak';
    matrix[10][0] = 'prettifyTime';
    matrix[11][0] = 'prettifyMem';
    matrix[12][0] = 'prettifyMemPeak';
    for (let index = 0; index < results.length; index++) {
      console.log(index);
      matrix[0][index + 1] = results[index].name;
      matrix[4][index + 1] = results[index].parseTime / 3;
      matrix[5][index + 1] = results[index].parseMem / (3 * 1024 * 1024);
      matrix[6][index + 1] = results[index].parseMemPeak / (1024 * 1024);
      matrix[7][index + 1] = results[index].stringifyTime / 3;
      matrix[8][index + 1] = results[index].stringifyMem / (3 * 1024 * 1024);
      matrix[9][index + 1] = results[index].stringifyMemPeak / (1024 * 1024);
      matrix[10][index + 1] = results[index].prettifyTime / 3;
      matrix[11][index + 1] = results[index].prettifyMem / (3 * 1024 * 1024);
      matrix[12][index + 1] = results[index].prettifyMemPeak / (1024 * 1024);

      matrix[1][index + 1] = (matrix[5][index + 1] + matrix[8][index + 1] + matrix[11][index + 1]) / (matrix[11][index + 1] == 0 ? 2 : 3);
      matrix[2][index + 1] = Math.max(matrix[6][index + 1], matrix[9][index + 1], matrix[12][index + 1]);
      matrix[3][index + 1] = results[index].fileSize / 1024;
    }
    console.log(matrix);
    stringify(matrix, function (err, output) {
      fs.writeFileSync('benchmark_template_json.csv', output.toString());
    });


  });
}

main();