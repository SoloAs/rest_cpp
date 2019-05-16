let chai = require("chai");
let chaiHttp = require("chai-http");
let should = chai.should();
let exec = require("child_process").exec;
let child;

let apiVersion = "/v1.0";
let server = "localhost:8081";
let executable = "./../../bin/rest_cpp";

chai.use(chaiHttp);


// start RESTful service
child = exec(executable, function (error, stdout, stderr) {
  if (error !== null) {
    console.log("Execution error: " + error);
    console.log("\x1b[31m", "Shutting down test suite");
    process.exit(0);
  }
});

describe("Starting RESTful Service", () => {
  before((done) => {
    setTimeout(() => {
        done();
    }, 3000)
  })
  describe("/GET baseurl", () => {
    it("it should GET available resources list", (done) => {
      chai.request(server + apiVersion)
          .get("/")
          .end((err, res) => {
            res.should.have.status(200);
            res.body.should.be.jsonFile;
            done();
          });
    });
  });
});

describe("STATUS", () => {
  describe("/GET status", () => {
    it("it should return status of module and hardware/firmware versions", (done) => {
      chai.request(server + apiVersion)
          .get("/status")
          .end((err, res) => {
            res.should.have.status(200);
            res.body.should.be.jsonFile;
            res.body.should.have.property("firmware_version");
            res.body.should.have.property("software_version");
            done();
          });
    });
  });
});

describe("INITIALIZATION", () => {
  describe("/POST init", () => {
    it("it should initialize the board", (done) => {
      chai.request(server + apiVersion)
        .post("/init")
        .end((err, res) => {
          res.should.have.status(201);
        })
    });
  });
});

describe("RESET", () => {
  describe("/POST reset", () => {
    it("it should reset the board", (done) => {
      chai.request(server + apiVersion)
        .post("/reset")
        .end((err, res) => {
          res.should.have.status(201);
        })
    });
  });
});

describe("CONFIG", () => {
  describe("/GET config", () => {
    it("it should return current configuration of the board", (done) => {
      chai.request(server + apiVersion)
        .get("/config")
        .end((err, res) => {
          res.should.have.status(200);
          res.body.should.be.jsonFile;
          done();
        })
    });
  });

  describe("/POST config", () => {
    it("it should change current configuration of the board", (done) => {
      let config = {
        "company": "Natchez Trace",
        "dhcp": true,
        "dns": "8.8.8.8",
        "igmp": true,
        "ip": "192.168.59.11",
        "mode": "RX"
      }
      chai.request(server + apiVersion)
        .post("/config")
        .send(config)
        .end((err, res) => {
          res.should.have.status(201);
          done();
        })
    });
  });

  describe("/PATCH config", () => {
    it("it should update current configuration of the board", (done) => {
      let config = {
        "company": "Sony",
      }
      chai.request(server + apiVersion)
        .patch("/config")
        .send(config)
        .end((err, res) => {
          res.should.have.status(200);
          done();
        })
    });
  });

  describe("/DELETE config", () => {
    it("it should set default configuration of the board", (done) => {
      chai.request(server + apiVersion)
        .delete("/config")
        .end((err, res) => {
          res.should.have.status(200);
          res.body.should.be.jsonFile;
          done();
        })
    });
  });
});

describe("STREAMS", () => {

  let stream_id = "stream0";
  let mode;

  chai.request(server + apiVersion)
    .get("config")
    .end((err, res) => {
      mode = res.body.mode;
    })

  describe("/GET streams", () => {
    it("it should return array of existent streams", (done) => {
      chai.request(server + apiVersion)
        .get("/streams")
        .end((err, res) => {
          res.should.have.status(200);
          res.should.be.an('array');
          done();
        })
    });
  });

  describe("/PUT stream", () => {
    it("it should create single stream", (done) => {
      let stream;
      if (mode == "RX") {
        stream = {
          "destination_port": "9999",
          "playout_delay": "3",
          "smpte_type": "1",
          "media_channel": "2",
          "audio_channel": "4",
          "frame_rate": "60",
          "video_format": "1080p",
          "destination_address": "192.168.59.13"
        }
      } else {
        stream = {
          "destination_port": "9999",
          "source port": "8888",
          "ttl": "60",
          "smpte_type": "1",
          "media_channel": "2",
          "audio_channel": "4",
          "vlan": "1",
          "video_format": "1080p",
          "audio_format": "2",
          "destination_address": "192.168.59.13"
        }
      }
      chai.request(server + apiVersion)
        .put("/streams/" + stream_id)
        .send(stream)
        .end((err, res) => {
          res.should.have.status(200);
          res.should.be.jsonFile;
          done();
        })
    });
  });
  

  describe("/GET stream", () => {
    it("it should return single existent stream", (done) => {
      chai.request(server + apiVersion)
        .get("/streams/" + stream_id)
        .end((err, res) => {
          res.should.have.status(200);
          res.should.be.jsonFile;
          res.should.have.property("id").equal(stream_id);
          done();
        })
    });
  });

  describe("/DELETE stream", () => {
    it("it should remove existent stream", (done) => {
      chai.request(server + apiVersion)
        .delete("/streams/" + stream_id)
        .end((err, res) => {
          res.should.have.status(204);
          done();
        })
    });
  });

});

// dismiss RESTful service
describe("EXIT", () => {
  describe("/GET exit", () => {
    it("it should shut down RESTful API", (done) => {
      chai.request(server)
        .get("/exit")
        .end((err, res) => {
          done();
        });
    });
  });
});




// describe("INITIALIZATION", () => {
//   describe("/POST init", () => {
//     it("it should initialize the board", (done) => {
      // chai.request(server + apiVersion)

//     });
//   });
// });
