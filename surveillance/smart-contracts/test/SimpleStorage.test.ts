import { should } from 'chai';
import { SimpleStorageInstance, SimpleStorageContract } from '../types/truffle-contracts';


const SimpleStorage = artifacts.require('./SimpleStorage.sol') as Truffle.Contract<SimpleStorageInstance>;
should();

/** @test {SimpleStorage} contract */
contract('SimpleStorage', (accounts) => {
    it('add a camera', async () => {
        const simpleStorageInstance = await SimpleStorage.new() as SimpleStorageInstance;

        let totalCameras;
        totalCameras = await simpleStorageInstance.totalCameras();
        (totalCameras.toString()).should.be.equal('0');

        await simpleStorageInstance.addCamera(82342, { from: accounts[0] });

        totalCameras = await simpleStorageInstance.totalCameras();
        (totalCameras.toString()).should.be.equal('1');
    });

    it('add a picture', async () => {
        const simpleStorageInstance = await SimpleStorage.new() as SimpleStorageInstance;

        await simpleStorageInstance.addCamera(82342, { from: accounts[0] });

        await simpleStorageInstance.addPicture(82342, 'QmS4ustL54uo8FzR9455qaxZwuMiUhyvMcX9Ba8nUH4uVv', { from: accounts[0] });
        await simpleStorageInstance.addPicture(82342, 'QmTT21QLauC8c31wvhuQ9Z1ActCYZRzrAnUy2rqPiF3X9a', { from: accounts[0] });
        
        const totalPictures = await simpleStorageInstance.totalPictures(82342);
        (totalPictures.toString()).should.be.equal('2');

        (await simpleStorageInstance.cameraPictures(82342, 0)).should.be.equal('QmS4ustL54uo8FzR9455qaxZwuMiUhyvMcX9Ba8nUH4uVv');
        (await simpleStorageInstance.cameraPictures(82342, 1)).should.be.equal('QmTT21QLauC8c31wvhuQ9Z1ActCYZRzrAnUy2rqPiF3X9a');
    });
});
