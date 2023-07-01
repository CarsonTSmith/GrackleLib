# LibGrackle

<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>


<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h3 align="center">LibGrackle README</h3>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

Grackle is an http like server library. An example of the format of Grackle messages is

00000039{"path":"/hello","jsonkey":"jsonvalue"}

The first 8 bytes correspond to the length of the message in bytes as a string. Following that we have the body of the message which is a Json string.
Every message should contain a path in the Json body.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.

### Prerequisites

To build this project you will need to install rapidjson-dev. You can get this from a package manager.
Rapidjson is a header only C++ json library that Grackle uses internally.

### Installation

1) Clone the repository
2) Build the project by issuing the shell command, "sh build.sh"
3) This will build the shared library. The libgrackle.so result can be found in the /lib directory.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage

Under the /example directory there is a sample program that uses the library to start a server up and create an endpoint.
You can build this program by calling, "make".
Please note that if there are any problems loading the libgrackle.so library, then you will need to add the libgrackle.so path to you 
LD_LIBRARY_PATH environment variable in your .bashrc file.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

1) Add a threadpool.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are appreciated.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement" or "bug".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See <a href="https://github.com/CarsonTSmith/LibGrackle/blob/main/LICENSE">License<a> for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Carson Smith - CarsonTSmith@gmail.com

Project Link: [https://github.com/CarsonTSmith/LibGrackle]([https://github.com/your_username/repo_name](https://github.com/CarsonTSmith/LibGrackle))

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments


<p align="right">(<a href="#readme-top">back to top</a>)</p>
